#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


/*
Compile:   g++ -O2 -std=c++17 a_star_8puzzle.cpp -o puzzle
Run:       ./puzzle h1 4 1 3 7 2 5 8 6 0
           ./puzzle h2 4 1 3 7 2 5 8 6 0
Input format: 9 integers for tiles row-major (0 = blank).
Heuristics:
  h1 -> #misplaced tiles
  h2 -> total Manhattan distance
Output:
  - Node/parent IDs, action, g/h/f at expansion and generation
  - Final path with states and stats
  - Edge list and optional Graphviz DOT to rebuild a search tree
*/

struct Node {
    array<uint8_t,9> s{};   // state
    int g=0, h=0, f=0;
    int parent=-1;          // index into "pool"
    char action='?';        // 'U','D','L','R'
    int id=-1;              // index into "pool"
};

static const array<uint8_t,9> GOAL = {1,2,3,4,5,6,7,8,0};

inline int manhattan(const array<uint8_t,9>& a){
    int d=0;
    for(int i=0;i<9;i++){
        uint8_t v=a[i];
        if(v==0) continue;
        int r=i/3,c=i%3;
        int gr=(v-1)/3,gc=(v-1)%3;
        d += abs(r-gr)+abs(c-gc);
    }
    return d;
}

inline int misplaced(const array<uint8_t,9>& a){
    int m=0;
    for(int i=0;i<9;i++) if(a[i]!=0 && a[i]!=GOAL[i]) m++;
    return m;
}

bool is_goal(const array<uint8_t,9>& a){ return a==GOAL; }

struct KeyHash {
    size_t operator()(const array<uint8_t,9>& a) const noexcept {
        // 64-bit FNV-1a-ish
        uint64_t h=1469598103934665603ull;
        for(auto v: a){ h ^= v; h *= 1099511628211ull; }
        return (size_t)h;
    }
};
struct KeyEq {
    bool operator()(const array<uint8_t,9>& x, const array<uint8_t,9>& y) const noexcept {
        return x==y;
    }
};

int inversion_count(const array<uint8_t,9>& a){
    int inv=0;
    for(int i=0;i<9;i++){
        if(a[i]==0) continue;
        for(int j=i+1;j<9;j++){
            if(a[j]==0) continue;
            if(a[i]>a[j]) inv++;
        }
    }
    return inv;
}
// For 3x3 with blank in any cell: solvable iff inversion parity is even.
bool solvable_3x3(const array<uint8_t,9>& a){
    return (inversion_count(a)%2)==0;
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc!=11){
        cerr << "Usage:\n  " << argv[0] << " <h1|h2> a1 a2 ... a9  (0 = blank)\n";
        return 1;
    }
    string hname = argv[1];
    function<int(const array<uint8_t,9>&)> H;
    if(hname=="h1") H = misplaced;
    else if(hname=="h2") H = manhattan;
    else {
        cerr << "Unknown heuristic '"<<hname<<"' (use h1 or h2)\n";
        return 1;
    }

    array<uint8_t,9> start{};
    for(int i=0;i<9;i++){
        int x = stoi(argv[2+i]);
        if(x<0 || x>8){ cerr<<"Tiles must be in [0..8]\n"; return 1;}
        start[i] = (uint8_t)x;
    }

    cout << "=== 8-Puzzle A* Search ===\n";
    cout << "Heuristic: " << (hname=="h1" ? "#misplaced (h1)" : "Manhattan (h2)") << "\n";
    auto print_state = [&](const array<uint8_t,9>& s){
        for(int r=0;r<3;r++){
            cout << "  ";
            for(int c=0;c<3;c++){
                int v=s[3*r+c];
                cout << (v?char('0'+v):' ') << (c==2?'\n':' ');
            }
        }
    };

    cout << "Start:\n"; print_state(start);
    cout << "Goal:\n";  print_state(GOAL);

    // Solvability check (useful for part (c) reasoning/logs)
    int inv = inversion_count(start);
    cout << "Inversions(start) = " << inv << " -> "
         << ((inv%2==0) ? "Solvable (even parity)\n" : "Unsolvable (odd parity)\n");

    if(!solvable_3x3(start)){
        cout << "This instance is NOT solvable. Exiting early.\n";
        return 0;
    }

    // A* data
    vector<Node> pool; pool.reserve(200000);
    auto make_node = [&](const array<uint8_t,9>& s, int g, int h, int parent, char act)->int{
        Node n; n.s=s; n.g=g; n.h=h; n.f=g+h; n.parent=parent; n.action=act;
        n.id = (int)pool.size();
        pool.push_back(n);
        return n.id;
    };

    struct PQItem {
        int id; int f; int h; uint64_t tie;
        bool operator<(const PQItem& o) const {
            // min-heap behavior in priority_queue requires reversed comparison
            if(f!=o.f) return f>o.f;
            if(h!=o.h) return h>o.h;      // prefer smaller h (best-first tie-break)
            return tie>o.tie;             // FIFO-ish among equals
        }
    };

    priority_queue<PQItem> open;
    unordered_map<array<uint8_t,9>, int, KeyHash, KeyEq> best_g; // state -> best g
    vector<pair<int,int>> edges; // (parent -> child) for tree reconstruction
    vector<string> edge_lbl;     // labels "move g/h/f"

    auto h0 = H(start);
    int start_id = make_node(start,0,h0,-1,'S');
    open.push({start_id, pool[start_id].f, pool[start_id].h, 0});
    best_g[start] = 0;

    cout << "\n--- SEARCH LOG ---\n";
    cout << "push id="<<start_id<<" f="<<pool[start_id].f<<" g=0 h="<<pool[start_id].h<<" action=S\n";

    // Movement
    const int dr[4]={-1,1,0,0};
    const int dc[4]={0,0,-1,1};
    const char mv[4]={'U','D','L','R'};

    int expanded=0, generated=1, max_frontier=1;
    uint64_t tick=1;
    int goal_id=-1;

    while(!open.empty()){
        max_frontier = max<int>(max_frontier, (int)open.size());
        auto cur = open.top(); open.pop();
        Node& u = pool[cur.id];

        // Skip if stale (had a worse g than what we know now)
        auto it = best_g.find(u.s);
        if(it==best_g.end() || it->second != u.g) continue;

        cout << "\nexpand id="<<u.id<<" parent="<<u.parent
             <<" action="<<u.action<<" g="<<u.g<<" h="<<u.h<<" f="<<u.f<<"\n";
        print_state(u.s);
        expanded++;

        if(is_goal(u.s)){ goal_id = u.id; break; }

        // find blank
        int zi = -1;
        for(int i=0;i<9;i++) if(u.s[i]==0) { zi=i; break; }
        int zr=zi/3, zc=zi%3;

        for(int k=0;k<4;k++){
            int nr=zr+dr[k], nc=zc+dc[k];
            if(nr<0||nr>=3||nc<0||nc>=3) continue;
            int ni = 3*nr + nc;

            array<uint8_t,9> v = u.s;
            swap(v[zi], v[ni]);

            int newg = u.g + 1;
            auto jt = best_g.find(v);
            if(jt!=best_g.end() && jt->second <= newg) {
                // We already have a better or equal g for this state
                continue;
            }
            int newh = H(v);
            int vid = make_node(v, newg, newh, u.id, mv[k]);

            best_g[v] = newg;
            open.push({vid, newg+newh, newh, tick++});
            generated++;

            // edge for tree
            edges.emplace_back(u.id, vid);
            {
                ostringstream oss;
                oss << mv[k] << " g="<<newg<<" h="<<newh<<" f="<<(newg+newh);
                edge_lbl.push_back(oss.str());
            }

            cout << "  generate id="<<vid<<" from parent="<<u.id
                 <<" via "<<mv[k]<<"  g="<<newg<<" h="<<newh<<" f="<<(newg+newh)<<"\n";
        }
    }

    cout << "\n--- RESULT ---\n";
    if(goal_id==-1){
        cout << "No solution found (should not happen for solvable input).\n";
        return 0;
    }

    // Reconstruct path
    vector<int> path;
    for(int x=goal_id; x!=-1; x=pool[x].parent) path.push_back(x);
    reverse(path.begin(), path.end());

    cout << "Solution length (optimal moves) = " << (int)path.size()-1 << "\n";
    cout << "Expanded nodes = " << expanded << "\n";
    cout << "Generated nodes = " << generated << "\n";
    cout << "Max frontier size = " << max_frontier << "\n";

    cout << "\nPath (state, action->next, g/h/f):\n";
    for(size_t i=0;i<path.size();i++){
        int id = path[i];
        Node& n = pool[id];
        cout << "id="<<id<<" g="<<n.g<<" h="<<n.h<<" f="<<n.f
             <<" action="<<n.action<<" (from parent="<<n.parent<<")\n";
        print_state(n.s);
        if(i+1<path.size()){
            cout << "  --" << pool[path[i+1]].action << "-->\n";
        }
    }

    // Edge list (good for building a search tree in another tool / spreadsheet)
    cout << "\n--- EDGE LIST (parent_id, child_id, label) ---\n";
    for(size_t i=0;i<edges.size();i++){
        cout << edges[i].first << "," << edges[i].second << "," << edge_lbl[i] << "\n";
    }

    // Optional Graphviz DOT (uncomment to use)
    /*
    cout << "\n--- GRAPHVIZ DOT ---\n";
    cout << "digraph AStar8Puzzle {\n";
    cout << "  rankdir=LR;\n";
    for (auto &n : pool){
        cout << "  N"<<n.id<<" [label=\"id="<<n.id<<"\\ng="<<n.g<<" h="<<n.h<<" f="<<n.f<<"\\naction="<<n.action<<"\"];\n";
    }
    for(size_t i=0;i<edges.size();i++){
        cout << "  N"<<edges[i].first<<" -> N"<<edges[i].second
             <<" [label=\""<<edge_lbl[i]<<"\"];\n";
    }
    cout << "}\n";
    */
    return 0;
}

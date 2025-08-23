#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// --- paste your parseIPv4 / ipToUint / matchRule here ---
#include <bitset>

static inline bool parseIPv4(const string& ip, uint32_t& out) {
  uint32_t a, b, c, d;
  char d1 = '.', d2 = '.', d3 = '.';
  stringstream ss(ip);
  if (!(ss >> a >> d1 >> b >> d2 >> c >> d3 >> d)) return false;
  if (d1 != '.' || d2 != '.' || d3 != '.') return false;
  if (a > 255 || b > 255 || c > 255 || d > 255) return false;
  out = (a << 24) | (b << 16) | (c << 8) | d;
  return true;
}

uint32_t ipToUint(const string& ip) {
  uint32_t v = 0;
  if (!parseIPv4(ip, v)) {
    // handle invalid IP as you wish; here we just return 0
    // but better: throw / return optional / mark non-matching
  }
  return v;
}

bool matchRule(const string& ip, const string& cidr) {
  uint32_t ipInt;
  if (!parseIPv4(ip, ipInt)) return false;  // or decide your policy

  size_t pos = cidr.find('/');
  const string baseIp = (pos == string::npos) ? cidr : cidr.substr(0, pos);
  const uint32_t mask = (pos == string::npos)
                            ? 32u
                            : static_cast<uint32_t>(stoi(cidr.substr(pos + 1)));
  if (mask > 32u) return false;  // invalid prefix

  uint32_t cidrInt;
  if (!parseIPv4(baseIp, cidrInt)) return false;

  const uint32_t maskBits = (mask == 0u) ? 0u : ~((1u << (32 - mask)) - 1u);

  // Debug (correct width):
  // cerr << "ipInt    " << bitset<32>(ipInt)   << "\n"
  //      << "cidrInt  " << bitset<32>(cidrInt) << "\n"
  //      << "maskBits " << bitset<32>(maskBits) << "\n";

  return (ipInt & maskBits) == (cidrInt & maskBits);
}

int main() {
  struct TC {
    string ip, cidr;
    bool expect;
    const char* note;
  };

  vector<TC> cases = {
      // ===== Basic /32 exact match =====
      {"1.2.3.4", "1.2.3.4", true, "exact /32 match"},
      {"1.2.3.5", "1.2.3.4", false, "exact /32 non-match"},

      // ===== Simple /24 =====
      {"192.168.1.5", "192.168.1.0/24", true, "in same /24"},
      {"192.168.2.5", "192.168.1.0/24", false, "different /24"},
      {"10.0.0.255", "10.0.0.0/24", true, "last IP in /24"},
      {"10.0.1.0", "10.0.0.0/24", false, "next /24"},

      // ===== /30 with non-network base (normalizes down) =====
      // 192.168.100.5/30 -> network is 192.168.100.4..7
      {"192.168.100.4", "192.168.100.5/30", true, "boundary start of /30"},
      {"192.168.100.5", "192.168.100.5/30", true, "inside /30"},
      {"192.168.100.6", "192.168.100.5/30", true, "inside /30"},
      {"192.168.100.7", "192.168.100.5/30", true, "boundary end of /30"},
      {"192.168.100.8", "192.168.100.5/30", false, "outside /30"},

      // ===== /31 (two addresses) RFC 3021 semantics for point-to-point =====
      // We only do pure mask compare: covers exactly two addresses
      {"100.100.100.0", "100.100.100.0/31", true, "/31 first"},
      {"100.100.100.1", "100.100.100.0/31", true, "/31 second"},
      {"100.100.100.2", "100.100.100.0/31", false, "outside /31"},

      // ===== /0 (match anything) =====
      {"0.0.0.0", "0.0.0.0/0", true, "any matches"},
      {"255.255.255.255", "1.2.3.4/0", true, "any matches"},
      {"123.45.67.89", "9.9.9.9/0", true, "any matches"},

      // ===== /32 boundaries =====
      {"0.0.0.0", "0.0.0.0/32", true, "exact 0.0.0.0"},
      {"0.0.0.1", "0.0.0.0/32", false, "just off"},
      {"255.255.255.255", "255.255.255.255/32", true, "exact broadcast"},
      {"255.255.255.254", "255.255.255.255/32", false, "just off"},

      // ===== Different prefix sizes =====
      {"172.16.5.7", "172.16.0.0/16", true, "inside /16"},
      {"172.17.0.1", "172.16.0.0/16", false, "outside /16"},
      {"203.0.113.128", "203.0.113.0/25", false, "upper half not in lower /25"},
      {"203.0.113.127", "203.0.113.0/25", true, "lower half /25"},
      {"203.0.113.127", "203.0.113.128/25", false,
       "lower half not in upper /25"},
      {"203.0.113.200", "203.0.113.128/25", true, "upper half /25"},

      // ===== Leading zeros / extra spaces (parser behavior: decimal) =====
      {"001.002.003.004", "1.2.3.4", true, "leading zeros still decimal"},
      {"  1.2.3.4", "1.2.3.4", true, "leading space ip"},
      {"1.2.3.4   ", "1.2.3.4", true, "trailing space ip"},
      {"1.2.3.4", " 1.2.3.4/32", true, "leading space cidr base"},
      {"1.2.3.4", "1.2.3.4 /32", false,
       "space before slash breaks strict parser"},

      // ===== Invalid IPs (octets out of range) =====
      {"256.0.0.1", "256.0.0.1/32", false, "invalid both sides"},
      {"1.2.3.4", "256.0.0.1/32", false, "invalid rule base"},
      {"300.2.3.4", "1.2.3.0/24", false, "invalid probe ip"},
      {"1.2.3", "1.2.3.0/24", false, "missing octet"},
      {"1.2.3.4.5", "1.2.3.0/24", false, "extra octet"},
      {"a.b.c.d", "1.2.3.0/24", false, "non-numeric ip"},

      // ===== Invalid masks =====
      {"1.2.3.4", "1.2.3.0/33", false, "mask > 32"},
      {"1.2.3.4", "1.2.3.0/-1", false, "negative mask (stoi -> -1 then cast)"},

      // ===== Random realistic private ranges =====
      {"192.168.0.1", "192.168.0.0/16", true, "inside 192.168.0.0/16"},
      {"192.169.0.1", "192.168.0.0/16", false, "outside 192.168.0.0/16"},
      {"10.1.2.3", "10.0.0.0/8", true, "inside 10.0.0.0/8"},
      {"11.0.0.1", "10.0.0.0/8", false, "outside 10.0.0.0/8"},

      // ===== Network vs host semantics (we do pure mask compare) =====
      // Even if base IP isn't aligned, mask compare should match the normalized
      // network.
      {"203.0.113.4", "203.0.113.5/30", true, "normalizes to .4/30"},
      {"203.0.113.8", "203.0.113.5/30", false, "next /30 block"},

      // ===== Boundary wrap safety (no overflow in compare) =====
      {"255.255.255.255", "255.255.255.252/30", true,
       "last /30 block matches .252-.255"},
      {"255.255.255.251", "255.255.255.252/30", false, "just before block"},
  };

  int pass = 0, fail = 0;
  for (size_t i = 0; i < cases.size(); ++i) {
    const auto& t = cases[i];
    bool got = matchRule(t.ip, t.cidr);
    bool ok = (got == t.expect);
    cout << (ok ? "[PASS] " : "[FAIL] ") << i + 1 << ": "
         << "IP=\"" << t.ip << "\" CIDR=\"" << t.cidr << "\" -> "
         << (got ? "true" : "false") << " | expect "
         << (t.expect ? "true" : "false") << "  -- " << t.note << "\n";
    ok ? ++pass : ++fail;
  }

  cout << "\nSummary: " << pass << " passed, " << fail << " failed, "
       << "total " << cases.size() << "\n";

  return fail == 0 ? 0 : 1;
}

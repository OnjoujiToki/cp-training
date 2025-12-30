auto convert = [&](std::string& s, int b) {
  for (char& c : s) {
    c -= '0';
  }

  std::vector<int> ans;
  while (!s.empty()) {
    std::string ns;
    int rem = 0;
    for (char c : s) {
      rem = rem * 10 + c;
      int quotient = rem / b;
      if (quotient || !ns.empty()) {
        ns.push_back(quotient);
      }
      rem = rem % b;
    }
    ans.push_back(rem);
    s = std::move(ns);
  }
  std::ranges::reverse(ans);
  return ans;
};
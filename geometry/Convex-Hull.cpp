// p -> q -> r: turn left(1), turn right(-1)
int cross(const pair<ll, ll>& p, const pair<ll, ll>& q, const pair<ll, ll>& r) {
    ll pqx = q.first - p.first;
    ll pqy = q.second - p.second;
    ll prx = r.first - p.first;
    ll pry = r.second - p.second;
    ll op = pqx * pry - pqy * prx;
    if(op > 0) return 1;
    if(op < 0) return -1;
    return 0;
}

// exactly 180deg -> not included
// mode = 0: full
// mode = 1: upper
// mode = 2: lower
vector<int> convex_hull(vector<pair<ll,ll>> &p, int mode = 0) {
    int n = p.size();
    if (n == 1) return {0};
    if (n == 2) {
        if (p[0] < p[1]) return {0, 1};
        if (p[1] < p[0]) return {1, 0};
        return {0};
    }
    vector<int> idx(n); iota(ALL(idx), 0);
    sort(ALL(idx), [&](int i, int j){ return p[i] < p[j]; } );
    auto scan = [&] () {
        vector<int> v;
        for (int k : idx) {
            while (v.size() > 1uz) {
                int i = v[v.size() - 2uz];
                int j = v[v.size() - 1uz];
                if (cross(p[i], p[j], p[k]) == 1) break;
                v.pop_back();
            }
            v.emplace_back(k);
        }
        return v;
    };
    vector<int> res;
    if (mode == 0 or mode == 2) {
        vc<int> lower = scan();
        res.insert(res.end(), lower.begin(), lower.end());
    }
    if (mode == 0 or mode == 1) {
        if (!res.empty()) res.pop_back();
        reverse(idx.begin(), idx.end());
        vc<int> upper = scan();
        res.insert(res.end(), upper.begin(), upper.end());
    }
    if (mode == 1) reverse(res.begin(), res.end());
    while (p.size() >= 2uz and p[res.front()] == p[res.back()]) res.pop_back();
    return res;
}

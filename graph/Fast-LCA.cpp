struct PlusMinusOneRMQ {
    int s;
    vector<int> v;
    vector<int> block_min_index, block_pm_bit, lg_table;
    vector<vector<int>> spt;
    vector<vector<vector<int>>> lookup;

    explicit PlusMinusOneRMQ() = default;

    explicit PlusMinusOneRMQ(const vector<int> &v): v(v) {
        int n = v.size();
        s = max(1, (31 - __builtin_clz(n)) / 2);
        int b = (n + s - 1) / s;
        int lg = 32 - __builtin_clz(b);
        block_min_index.assign(b, -1);
        block_pm_bit.assign(b, 0);
        lg_table.assign(b + 1, 0);
        spt.assign(lg, vector<int>(b));
        lookup.assign(1 << (s - 1), vector<vector<int>> (s, vector<int> (s + 1)));
        for (int i = 2; i <= b; i++) lg_table[i] = lg_table[i >> 1] + 1;
        for (int i = 0, l = 0; i < b; i++, l += s) {
            int r = min(n, l + s);
            block_min_index[i] = l;
            for (int j = l + 1; j < r; j++) {
                if (v[j] < v[block_min_index[i]]) block_min_index[i] = j;
                if (v[j - 1] < v[j]) block_pm_bit[i] |= 1 << (j - l - 1);
            }
        }
        for (int i = 0; i < b; i++) spt[0][i] = block_min_index[i];
        for (int i = 1; i < lg; i++) {
            for (int j = 0; j + (1 << i) <= b; j++) {
                spt[i][j] = (v[spt[i - 1][j]] < v[spt[i - 1][j + (1 << (i - 1))]] ?
                        spt[i - 1][j] : spt[i - 1][j + (1 << (i - 1))]);
            }
        }
        for (int bit = 0; bit < (1 << (s - 1)); bit++) {
            for (int l = 0; l < s; l++) {
                int cur = 0, min_val = 0, min_pos = l;
                for (int r = l + 1; r <= s; r++) {
                    lookup[bit][l][r] = min_pos;
                    if (bit >> (r - 1) & 1) cur++;
                    else cur--;
                    if (cur < min_val) {
                        min_val = cur;
                        min_pos = r;
                    }
                }
            }
        }
    };

    inline int fold(int l, int r) const {
        assert(l < r);
        int li = l / s;
        int ri = r / s;
        if (li == ri) return lookup[block_pm_bit[li]][l % s][r % s] + li * s;
        int ret = lookup[block_pm_bit[li]][l % s][s] + li * s;
        if (r % s > 0) {
            int right = lookup[block_pm_bit[ri]][0][r % s] + ri * s;
            if (v[right] < v[ret]) ret = right;
        }
        if (li + 1 != ri) {
            int layer = lg_table[ri - li - 1];
            int mid = v[spt[layer][li + 1]] < v[spt[layer][ri - (1 << layer)]] ?
                spt[layer][li + 1] : spt[layer][ri - (1 << layer)];
            if (v[mid] < v[ret]) ret = mid;
        }
        return ret;
    }
};

struct RMQLowestCommonAncestor {
    vector<int> et,dep,tin;
    // using F = function<int(int,int)>;
    // SparseTable<int,F> st;
    PlusMinusOneRMQ st;
    const vector<vector<int>> &g;

    RMQLowestCommonAncestor (const vector<vector<int>> &g): g(g){}

    void init (int root = 0) {
        const int n = g.size();
        et.reserve(n*2-1);
        dep.reserve(n*2-1);
        tin.resize(n);
        dfs(root, -1, 0);
        // vector<int> vs(n*2-1);
        // iota(ALL(vs),0);
        // F f = [&](int a, int b) {return dep[a] < dep[b] ? a:b;};
        // st = SparseTable<int, F> (vs, f);
        st = PlusMinusOneRMQ(dep);
    }

    void dfs(int u, int p, int d){
        tin[u] = (int)et.size();
        et.emplace_back(u);
        dep.emplace_back(d);
        for(int v : g[u]) if(v != p){
            dfs(v, u, d+1);
            et.emplace_back(u);
            dep.emplace_back(d);
        }
    }

    int lca(int x, int y) const{
        if(tin[x] > tin[y]) swap(x,y);
        return x == y ? x : et[st.fold(tin[x],tin[y])];
    }

    int dist (int u, int v) const{
        int w = lca(u,v);
        return dep[tin[u]]+dep[tin[v]]-dep[tin[w]]*2;
    }
};

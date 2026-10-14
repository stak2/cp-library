template<typename T, typename F>
struct SparseTable {
    F f;
    vector<vector<T>> v;
    vector<int> lookup;

    SparseTable() = default;

    explicit SparseTable (const vector<T> &a, const F &f) : f(f){
        const int n = (int)a.size();
        const int lg = 32-__builtin_clz(n);
        v.assign(lg, vector<T>(n));
        for(int i = 0; i < n; i++) v[0][i] = a[i];
        for(int i = 1; i < lg; i++){
            for(int j = 0; j+(1<<i) <= n; j++){
                v[i][j] = f(v[i-1][j], v[i-1][j+(1<<(i-1))]);
            }
        }
        lookup.resize(n+1);
        for (int i = 2; i < (int)lookup.size(); i++) {
            lookup[i] = lookup[i >> 1] + 1;
        }
    }

    inline T fold(int l, int r) const {
        int b = lookup[r-l];
        return f(v[b][l], v[b][r-(1<<b)]);
    }
};

struct RMQLowestCommonAncestor {
    vector<int> et,dep,tin;
    using F = function<int(int,int)>;
    SparseTable<int,F> st;
    const vector<vector<int>> &g;

    RMQLowestCommonAncestor (const vector<vector<int>> &g): g(g){}

    void init (int root = 0) {
        const int n = g.size();
        et.reserve(n*2-1);
        dep.reserve(n*2-1);
        tin.resize(n);
        dfs(root, -1, 0);
        vector<int> vs(n*2-1);
        iota(ALL(vs),0);
        F f = [&](int a, int b) {return dep[a] < dep[b] ? a:b;};
        st = SparseTable<int,F>(vs,f);
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

struct AuxiliaryTree : RMQLowestCommonAncestor{
    vector<vector<int>> ret;
    AuxiliaryTree(const vector<vector<int>> &g) : RMQLowestCommonAncestor(g), ret(g.size()) {
        RMQLowestCommonAncestor::init();
    }

    int build(vector<int> &vs) {
        const int k = vs.size();
        if (k == 0) return -1;
        sort(ALL(vs), [&](int i, int j){
            return tin[i] < tin[j];
        });
        stack<int> stk;
        stk.emplace(vs[0]);
        for(int i = 0; i < k-1; i++){
            int w = lca(vs[i], vs[i+1]);
            if (w != vs[i]){
                int last = stk.top(); stk.pop();
                while (!stk.empty() and dep[tin[w]] < dep[tin[stk.top()]]){
                    ret[stk.top()].emplace_back(last);
                    last = stk.top();
                    stk.pop();
                }

                if (stk.empty() or stk.top() != w){
                    stk.emplace(w);
                    vs.emplace_back(w);
                }
                ret[w].emplace_back(last);
            }
            stk.emplace(vs[i+1]);
        }

        while(stk.size()>1){
            int l = stk.top(); stk.pop();
            ret[stk.top()].emplace_back(l);
        }
        return stk.top();
    }

    void clear(const vector<int> &vs){
        for(int v : vs) ret[v].clear();
    }
};

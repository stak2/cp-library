template<typename T, T (*f)(T, T)>
struct SparseTable {
    vector<vector<T>> v;
    vector<int> lookup;

    SparseTable() = default;

    explicit SparseTable (const vector<T> &a){
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

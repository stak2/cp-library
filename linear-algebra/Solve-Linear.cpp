template <typename T>
vector<vector<T>> solve_linear(vector<vector<T>> a, vector<T> b, int n = -1, int m = -1) {
    if (n == -1) {
        n = a.size();
        m = a[0].size();
    }
    assert(n > 0 and n == (int)a.size() and n == (int)b.size());

    int r = 0;
    for (int j = 0; j < m; j++) {
        if (r == n) break;
        for (int i = r; i < n; i++) if (a[i][j] != T(0)) {
            swap(a[r], a[i]);
            swap(b[r], b[i]);
            break;
        }
        if (a[r][j] == T(0)) continue;
        T c = T(1) / a[r][j];
        for (auto&& x : a[r]) x *= c;
        b[r] *= c;

        for (int i = 0; i < n; i++) if (i != r) {
            T c = a[i][j];
            if (c == T(0)) continue;
            b[i] -= b[r]*c;
            for (int k = j; k < m; k++) a[i][k] -= a[r][k]*c;
        }
        r++;
    }

    // no solution
    for (int i = r; i < n; i++) if (b[i] != T(0)) return {};

    // res[0] -> answer
    // res[1:] -> basis
    vector<vector<T>> res(1, vector<T> (m));
    vector<int> pivot(m, -1);
    int p = 0;
    for (int i = 0; i < r; i++) {
        while (a[i][p] == T(0)) p++;
        res[0][p] = b[i];
        pivot[p] = i;
    }
    for (int j = 0; j < m; j++) if (pivot[j] == -1) {
        vector<T> x(m);
        x[j] = T(-1);
        for (int k = 0; k < j; k++) if (pivot[k] != -1) x[k] = a[pivot[k]][j];
        res.emplace_back(x);
    }
    return res;
}


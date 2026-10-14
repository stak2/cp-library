template <typename T>
T determinant(vector<vector<T>> a) {
    int n = a.size();
    assert(n == (int)a[0].size());

    T det = T(1);
    for (int j = 0; j < n; j++) {
        for (int i = j; i < n; i++) if (a[i][j] != T(0)) {
            if (i != j) {
                swap(a[i], a[j]);
                det = -det;
            }
            break;
        }
        if (a[j][j] == T(0)) return 0;

        T c = T(1) / a[j][j];
        for (int i = j+1; i < n; i++) {
            T r = a[i][j] * c;
            for (int k = j; k < n; k++) {
                a[i][k] -= r * a[j][k];
            }
        }
    }

    for (int i = 0; i < n; i++) det *= a[i][i];
    return det;
}


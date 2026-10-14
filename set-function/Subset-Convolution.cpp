template<typename T>
vector<vector<T>> ranked_zeta(vector<T> a) {
    int n = a.size();
    int lg = highbit(n);
    vector<vector<T>> za(n, vector<T>(lg + 1));
    for (int s = 0; s < n; s++) za[s][popcnt(s)] = a[s];
    for (int k = 0; k < lg; k++) {
        for (int s = 0; s < n; s++) if(s >> k & 1) {
            for (int i = 0; i <= lg; i++) za[s][i] += za[s ^ 1 << k][i];
        }
    }
    return za;
}

template<typename T>
vector<vector<T>> ranked_mobius(vector<T> a) {
    int n = a.size();
    int lg = highbit(n);
    vector<vector<T>> ma(n, vector<T>(lg + 1));
    for (int s = 0; s < n; s++) ma[s][popcnt(s)] = a[s];
    for (int k = 0; k < lg; k++) {
        for (int s = 0; s < n; s++) if(s >> k & 1) {
            for (int i = 0; i <= lg; i++) ma[s][i] -= ma[s ^ 1 << k][i];
        }
    }
    return ma;
}

template<typename T>
vector<T> subset_convolution(vector<T> a, vector<T> b) {
    int n = a.size();
    int lg = highbit(n);
    vector<vector<T>> za(n, vector<T>(lg + 1));
    for (int s = 0; s < n; s++) za[s][popcnt(s)] = a[s];
    for (int k = 0; k < lg; k++) {
        for (int s = 0; s < n; s++) if(s >> k & 1) {
            for (int i = 0; i <= lg; i++) za[s][i] += za[s ^ 1 << k][i];
        }
    }

    vector<vector<T>> zb(n, vector<T>(lg + 1));
    for (int s = 0; s < n; s++) zb[s][popcnt(s)] = b[s];
    for (int k = 0; k < lg; k++) {
        for (int s = 0; s < n; s++) if(s >> k & 1) {
            for (int i = 0; i <= lg; i++) zb[s][i] += zb[s ^ 1 << k][i];
        }
    }

    for (int s = 0; s < n; s++) {
        vc<T> c(lg + 1);
        for (int i = 0; i <= lg; i++) {
            for (int j = 0; j <= lg - i; j++) {
                c[i + j] += za[s][i] * zb[s][j];
            }
        }
        for (int i = 0; i <= lg; i++) za[s][i] = c[i];
    }

    vector<T> res(n);
    for (int k = 0; k < lg; k++) {
        for (int s = 0; s < n; s++) if(s >> k & 1) {
            for (int i = 0; i <= lg; i++) za[s][i] -= za[s ^ 1 << k][i];
        }
    }
    for (int s = 0; s < n; s++) res[s] = za[s][popcnt(s)];
    return res;
}

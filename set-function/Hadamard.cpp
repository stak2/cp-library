template <typename T>
void hadamard(vector<T> &f, bool inv = false) {
    int n = f.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int j = 0; j < n; j += i << 1) {
            for (int k = 0; k < i; k++) {
                T x = f[j + k], y = f[j + k + i];
                f[j + k] = x + y;
                f[j + k + i] = x - y;
            }
        }
    }
    if (inv) {
        T inv_n = T(1) / n;
        for (auto &x : f) x *= inv_n;
    }
}


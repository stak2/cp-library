template <typename T>
void hadamard(vector<T> &f, bool inv = false) {
    int n = f.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int s = 0; s < n; s++) {
            if (!(s & i)) {
                T x = f[s], y = f[s ^ i];
                f[s] = x + y, f[s ^ i] = x - y;
            }
        }
    }
    if (inv) {
        if constexpr (is_integral<T>::value) {
            for (auto &x : f) x /= n;
        }
        else {
            T inv_n = T(1) / T(n);
            for (auto &x : f) x *= inv_n;
        }
    }
}


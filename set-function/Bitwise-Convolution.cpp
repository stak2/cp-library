template <typename T>
vector<T> bitwise_or_convolution(vector<T> a, vector<T> b) {
    subset_zeta(a);
    subset_zeta(b);
    for (size_t i = 0; i < a.size(); i++) a[i] *= b[i];
    subset_mobius(a);
    return a;
}

template <typename T>
vector<T> bitwise_and_convolution(vector<T> a, vector<T> b) {
    superset_zeta(a);
    superset_zeta(b);
    for (size_t i = 0; i < a.size(); i++) a[i] *= b[i];
    superset_mobius(a);
    return a;
}

template <typename T>
vector<T> bitwise_xor_convolution(vector<T> f, vector<T> g) {
    hadamard(f, false); hadamard(g, false);
    for (size_t i = 0; i < f.size(); i++) f[i] *= g[i];
    hadamard(f, true);
    return f;
}

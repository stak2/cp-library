template <typename T>
vector<T> bitwise_or_convolution(vector<T> a, vector<T> b) {
    int n = a.size();
    subset_zeta(a);
    subset_zeta(b);
    for (int i = 0; i < n; i++) a[i] *= b[i];
    subset_mobius(a);
    return a;
}

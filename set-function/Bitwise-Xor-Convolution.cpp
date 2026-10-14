template <typename T>
vector<T> bitwise_xor_convolution(vector<T> f, vector<T> g) {
    assert(f.size() == g.size());
    const int n = int(f.size());
    assert((n & (n-1)) == 0);
    hadamard(f,false); hadamard(g,false);
    for(int i = 0; i < n; i++) f[i] *= g[i];
    hadamard(f, true);
    return f;
}

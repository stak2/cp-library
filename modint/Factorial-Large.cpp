// f(0), ..., f(n-1) -> f(c), ..., f(c+m-1)
template <typename mint>
vector<mint> sample_point_shift(const vector<mint> &f, mint c, int m) {
    int n = f.size();
    vector<mint> a(n),b(n);
    for (int i = 0; i < n; i++) {
        a[i] = f[i] * bc.finv(i);
        b[i] = bc.finv(i) * (i % 2 ? -1 : 1);
    }
    a = atcoder::convolution(a, b);
    a.resize(n);
    mint ff = 1;
    for (int i = 0; i < n; i++) {
        a[i] *= bc.fact(i);
        b[i] = ff * bc.finv(i);
        ff *= c - i;
    }
    reverse(a.begin(), a.end());
    a = atcoder::convolution(a, b);
    a.resize(n);
    reverse(a.begin(), a.end());
    for (int i = 0; i < n; i++) a[i] *= bc.finv(i);
    b.resize(m);
    for (int i = 0; i < m; i++) b[i] = bc.finv(i);
    a = atcoder::convolution(a, b);
    for (int i = 0; i < m; i++) b[i] = a[i] * bc.fact(i);
    return b;
}

template <typename mint>
struct Factorial {
    static constexpr int K = 9;
    static constexpr int B = 1 << K;
    static constexpr int L = mint::mod() >> K;
    vector<mint> blk;

    Factorial() : blk(L + 1) {
        vector<mint> f{1};
        f.reserve(B);
        for (int i = 0; i < K; i++) {
            vector<mint> g = sample_point_shift(f, mint(1 << i), 3 << i);
            f.resize(2 << i);
            const auto at = [&] (int j) { return j < (1 << i) ? f[j] : g[j - (1 << i)]; };
            for (int j = 0; j < (2 << i); j++) {
                f[j] = at(2 * j) * at(2 * j + 1) * (1 << i) * (2 * j + 1);
            }
        }
        if (B < L) {
            vector<mint> g = sample_point_shift(f, mint(B), L - B);
            move(g.begin(), g.end(), back_inserter(f));
        }
        else {
            f.resize(L);
        }
        blk[0] = 1;
        for (int i = 0; i < L; i++) blk[i + 1] = blk[i] * f[i] * (i + 1) * B;
    }

    mint calc(long long n) {
        assert(n >= 0);
        if (n >= mint::mod()) return 0;
        int q = n / B, r = n % B;
        mint ret;
        if (2 * r <= B) {
            ret = blk[q];
            for (int i = n - r + 1; i <= n; i++) ret *= i;
        }
        else if (q != L) {
            ret = blk[q + 1];
            mint over = 1;
            for (int i = n + 1; i <= n - r + B; i++) over *= i;
            ret /= over;
        }
        else {
            ret = mint(-1);
            mint over = 1;
            for (int i = n + 1; i < mint::mod(); i++) over *= i;
            ret /= over;
        }
        return ret;
    }
};


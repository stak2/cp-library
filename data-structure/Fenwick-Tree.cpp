template <typename S>
struct FenwickTree {
    int n;
    vector<S> dat;

    FenwickTree() = default;
    FenwickTree(int n_) : n(n_) { dat.resize(n); }
    FenwickTree(const vector<S> &a) : n((int)a.size()), dat(a) {
        for (int i = 1; i <= n; i++) {
            int j = i + (i & -i);
            if (j <= n) dat[j-1] += dat[i-1];
        }
    }

    void add(int p, S x) {
        assert(0 <= p and p < n);
        for (++p; p <= n; p += (p & -p)) dat[p-1] += x;
    }

    S sum(int l, int r) const {
        assert(0 <= l and l <= r and r <= n);
        if (l == r) return S(0);
        unsigned d = unsigned(l ^ r);
        int shift = 32 - countl_zero(d);
        int common = unsigned(l) & (~0u << shift);
        S sl = 0, sr = 0;
        for (; l != common; l &= l-1) sl += dat[l-1];
        for (; r != common; r &= r-1) sr += dat[r-1];
        return sr - sl;
    }
};


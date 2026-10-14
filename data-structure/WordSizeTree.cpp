struct WordSizeTree{
    using u32 = uint32_t;
    using u64 = uint64_t;
    inline int lowbit(u64 x) { return x ? __builtin_ctzll(x) : -1; }
    inline int highbit(u64 x) { return x ? 63 - __builtin_clzll(x) : -1; }

    static constexpr u32 B = 64;
    int n, log;
    vector<vector<u64>> seg;

    WordSizeTree() {}
    WordSizeTree(int n_) { build(n_); }

    template <typename F>
    WordSizeTree(int n_, F f) { build(n_, f); }

    void build(int sz) {
        seg.clear();
        n = sz;
        do {
            seg.emplace_back(vector<u64>((sz+B-1)/B));
            sz = (sz+B-1)/B;
        } while(sz > 1);
        log = seg.size();
    }
    template <typename F>
    void build(int sz, F f) {
        build(sz);
        for (int i = 0; i < n; i++) seg[0][i/B] |= u64(f(i) << (i%B));
        for (int h = 0; h < log-1; h++) {
            for (int i = 0; i < (int)seg[h].size(); i++) {
                seg[h+1][i/B] |= u64(bool(seg[h][i])) << (i%B);
            }
        }
    }

    int size() const { return n; }

    bool operator[] (int i) const { return seg[0][i/B] >> (i%B) & 1; }

    void insert(int i) {
        assert(0 <= i and i < n);
        for (int h = 0; h < log; h++) { seg[h][i/B] |= u64(1) << (i%B), i /= B; }
    }
    void erase(int i) {
        assert(0 <= i and i < n);
        u64 x = 0;
        for (int h = 0; h < log; h++) {
            seg[h][i/B] &= ~(u64(1) << (i%B));
            seg[h][i/B] |= x << (i%B);
            x = bool(seg[h][i/B]);
            i /= B;
        }
    }

    // min x s.t. (x >= i) and (x in S), or n if no such x exists
    int next(int i) {
        assert(i <= n);
        if (i < 0) i = 0;
        for (int h = 0; h < log; h++) {
            if (i/B == seg[h].size()) break;
            u64 d = seg[h][i/B] >> (i%B);
            if (!d) {
                i = i/B+1;
                continue;
            }
            i += lowbit(d);
            for (int g = h-1; g >= 0; g--) {
                i *= B;
                i += lowbit(seg[g][i/B]);
            }
            return i;
        }
        return n;
    }

    // max x s.t. (x <= i) and (x in S), or -1 if no such x exists
    int prev(int i) {
        assert(i >= -1);
        if (i >= n) i = n-1;
        for (int h = 0; h < log; h++) {
            if (i == -1) break;
            u64 d = seg[h][i/B] << (63-i%B);
            if (!d) {
                i = i/B-1;
                continue;
            }
            i -= __builtin_clzll(d);
            for (int g = h-1; g >= 0; g--) {
                i *= B;
                i += highbit(seg[g][i/B]);
            }
            return i;
        }
        return -1;
    }
};

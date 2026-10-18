struct BitVector {
    int blk;
    vector<u64> bit;
    vector<int> sum;
    BitVector() = default;
    explicit BitVector(int sz): blk((sz+63) >> 6) {
        bit.assign(blk, 0ULL);
        sum.assign(blk, 0);
    }
    void set(int idx) { bit[idx>>6] |= 1ULL << (idx & 63); }
    void build() {
        sum[0] = 0;
        for (int i = 1; i < blk; i++) sum[i] = sum[i-1] + popcount(bit[i-1]);
    }
    // count 'val' in [0,r)
    int rank(int r, bool val) const {
        int r1 = sum[r>>6] + popcount(bit[r>>6] & ((1ULL << (r&63))-1));
        return val ? r1 : r - r1;
    }
    // count 'val' in [l,r)
    int rank(int l, int r, bool val) const {
        return rank(r, val) - rank(l, val);
    }
};

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

template <typename T, int H, typename S>
struct FenwickTreeOnWaveletMatrix {
    BitVector bs[H];
    FenwickTree<S> ft[H+1];
    int mid[H];
    vector<T> v;

    FenwickTreeOnWaveletMatrix() = default;

    FenwickTreeOnWaveletMatrix(const vector<T> &v_, const vector<S> &w) : v(v_) {
        assert(v.size() == w.size());
        int n = v.size();
        vector<int> l(n), r(n), ord(n);
        iota(ord.begin(), ord.end(), 0);
        vector<S> ws(n);
        ft[H] = FenwickTree(w);
        for (int h = H-1; h >= 0; h--) {
            bs[h] = BitVector(n+1);
            int lp = 0, rp = 0;
            for (int i = 0; i < n; i++) {
                if (v[ord[i]]>>h&1) {
                    bs[h].set(i);
                    r[rp++] = ord[i];
                }
                else {
                    l[lp++] = ord[i];
                }
            }
            mid[h] = lp;
            bs[h].build();
            swap(ord, l);
            for (int i = 0; i < rp; i++) ord[lp+i] = r[i];
            for (int i = 0; i < n; i++) ws[i] = w[ord[i]];
            ft[h] = FenwickTree(ws);
        }
    }

    pair<int, int> descend(int h, int l, int r, bool val) const {
        return {bs[h].rank(l, val) + mid[h] * val, bs[h].rank(r, val) + mid[h] * val};
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [0,u)
    S rect_sum(int l, int r, T u) const {
        if (l == r or u <= T(0)) return S(0);
        if (u >= (T(1) << H)) return ft[H].sum(l, r);
        S ret = 0;
        for (int h = H-1; h >= 0; h--) {
            bool uh = u>>h&1;
            auto [l0, r0] = descend(h, l, r, 0);
            if(uh) {
                ret += ft[h].sum(l0, r0);
                l = l - l0 + mid[h];
                r = r - r0 + mid[h];
            }
            else {
                l = l0; r = r0;
            }
            if (l == r) break;
        }
        return ret;
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [d,u)
    S rect_sum(int l, int r, T d, T u) const {
        return rect_sum(l, r, u) - rect_sum(l, r, d);
    }

    // w[idx] += x
    void point_add(int idx, T x) {
        T cur = v[idx];
        ft[H].add(idx, x);
        for (int h = H-1; h >= 0; h--) {
            bool ch = cur>>h&1;
            idx = bs[h].rank(idx, ch) + mid[h] * ch;
            ft[h].add(idx, x);
        }
    }

    // [l,r) × [0,y) 内のデータ件数 cnt、重み和 s とする
    // atcoder::segtree の max_right の要領で二分探索を行う
    // f(y, cnt, s) が true となる最大の y を求め、そのときの y,cnt,s を返す
    // f は y について単調
    template <typename F>
    tuple<T, int, S> max_right(int l, int r, F f) const {
        S s_all = ft[H].sum(l, r);
        if (f(T(1)<<H, r-l, s_all)) return {T(1)<<H, r-l, s_all};
        T t = T(0);
        int cnt = 0;
        S s = S(0);
        for (int h = H-1; h >= 0; h--) {
            auto [l0, r0] = descend(h, l, r, 0);
            auto [l1, r1] = descend(h, l, r, 1);
            int cnt_tmp = cnt + r0 - l0;
            S s_tmp = s + ft[h].sum(l0, r0);
            T t_tmp = t | T(1) << h;
            if (f(t_tmp, cnt_tmp, s_tmp)) {
                t = t_tmp; cnt = cnt_tmp; s = s_tmp;
                l = l1; r = r1;
            }
            else {
                l = l0; r = r0;
            }
        }
        return {t, cnt, s};
    }

    // [l,r) × [y,inf) 内のデータ件数 cnt、重み和 s とする
    // atcoder::segtree の min_left の要領で二分探索を行う
    // f(y, cnt, s) が true となる最小の y を求め、そのときの y,cnt,s を返す
    // f は y について単調
    template <typename F>
    tuple<T, int, S> min_left(int l, int r, F f) const {
        S s_all = ft[H].sum(l, r);
        if (f(0, r-l, s_all)) return {0, r-l, s_all};
        T t = T(1) << H;
        int cnt = 0;
        S s = S(0);
        for (int h = H-1; h >= 0; h--) {
            auto [l0, r0] = descend(h, l, r, 0);
            auto [l1, r1] = descend(h, l, r, 1);
            int cnt_tmp = cnt + r1 - l1;
            S s_tmp = s + ft[h].sum(l1, r1);
            T t_tmp = t - (T(1) << h);
            if (f(t_tmp, cnt_tmp, s_tmp)) {
                t = t_tmp; cnt = cnt_tmp; s = s_tmp;
                l = l0; r = r0;
            }
            else {
                l = l1; r = r1;
            }
        }
        return {t, cnt, s};
    }
};

template <typename T, int H, typename S>
struct FenwickTreeOnCompressedWaveletMatrix {
    FenwickTreeOnWaveletMatrix<int, H, S> wm;
    vector<T> raw;

    FenwickTreeOnCompressedWaveletMatrix(const vector<T> &v, const vector<S> &w) : raw(v) {
        sort(raw.begin(), raw.end());
        raw.erase(unique(raw.begin(), raw.end()), raw.end());
        vector<int> vi(v.size());
        for (size_t i = 0; i < v.size(); i++) vi[i] = get_idx(v[i]);
        wm = FenwickTreeOnWaveletMatrix<int, H, S>(vi, w);
    }

    inline int get_idx(T x) const {
        return lower_bound(raw.begin(), raw.end(), x) - raw.begin();
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [0,u)
    S rect_sum(int l, int r, T u) const {
        return wm.rect_sum(l, r, get_idx(u));
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [d,u)
    S rect_sum(int l, int r, T d, T u) const {
        return wm.rect_sum(l, r, get_idx(d), get_idx(u));
    }

    // w[idx] += x
    void point_add(int idx, T x) {
        wm.point_add(idx, x);
    }

    template <typename F>
    tuple<T, int, S> max_right(int l, int r, F f, T inf) const {
        S s_all = wm.ft[H].sum(l, r);
        if (f(inf, r-l, s_all)) return {inf, r-l, s_all};
        auto fi = [&] (int idx, int cnt, S s) {
            return f(idx >= (int)raw.size() ? inf : raw[idx], cnt, s);
        };
        auto [t, cnt, s] = wm.max_right(l, r, fi);
        return {t >= (int)raw.size() ? inf : raw[t], cnt, s};
    }

    template <typename F>
    tuple<T, int, S> min_left(int l, int r, F f, T inf) const {
        S s_all = wm.ft[H].sum(l, r);
        if (f(-inf, r-l, s_all)) return {-inf, r-l, s_all};
        auto fi = [&] (int idx, int cnt, S s) {
            return f(idx >= (int)raw.size() ? inf : raw[idx], cnt, s);
        };
        auto [t, cnt, s] = wm.min_left(l, r, fi);
        assert(0 <= t and t <= (int)raw.size());
        return {t == 0 ? -inf : raw[t-1] + 1, cnt, s};
    }
};

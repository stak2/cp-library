#include <cstdio>
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

template <typename T, int H, typename S>
struct WaveletMatrixRectangleSum {
    BitVector bs[H];
    vector<S> ps[H+1];
    int mid[H];

    WaveletMatrixRectangleSum() = default;
    WaveletMatrixRectangleSum(const vector<T> &v, const vector<S> &w) {
        assert(v.size() == w.size());
        int n = v.size();
        vector<int> l(n), r(n), ord(n);
        iota(ord.begin(), ord.end(), 0);
        ps[H].resize(n+1);
        for (int i = 0; i < n; i++) ps[H][i+1] = ps[H][i] + w[i];
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
            ps[h].resize(n+1);
            ps[h][0] = S(0);
            for (int i = 0; i < n; i++) ps[h][i+1] = ps[h][i] + w[ord[i]];
        }
    }

    pair<int, int> descend(int h, int l, int r, bool val) const {
        return {bs[h].rank(l, val) + mid[h] * val, bs[h].rank(r, val) + mid[h] * val};
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [0,u)
    S rect_sum(int l, int r, T u) const {
        if (l == r or u <= T(0)) return S(0);
        if (u >= (T(1) << H)) return ps[H][r] - ps[H][l];
        S ret = 0;
        for (int h = H-1; h >= 0; h--) {
            bool uh = u>>h&1;
            auto [l0, r0] = descend(h, l, r, 0);
            if (uh) {
                ret += ps[h][r0] - ps[h][l0];
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

    template <typename F>
    tuple<T, int, S> max_right(int l, int r, F f) {
        S s_all = ps[H][r] - ps[H][l];
        if (f(T(1)<<H, r-l, s_all)) return {T(1)<<H, r-l, s_all};
        T t = T(0);
        int cnt = 0;
        S s = S(0);
        for (int h = H-1; h >= 0; h--) {
            auto [l0, r0] = descend(h, l, r, 0);
            auto [l1, r1] = descend(h, l, r, 1);
            int cnt_tmp = cnt + r0 - l0;
            S s_tmp = s + ps[h][r0] - ps[h][l0];
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

    template <typename F>
    tuple<T, int, S> min_left(int l, int r, F f) {
        S s_all = ps[H][r] - ps[H][l];
        if (f(0, r-l, s_all)) return {0, r-l, s_all};
        T t = T(1) << H;
        int cnt = 0;
        S s = S(0);
        for (int h = H-1; h >= 0; h--) {
            auto [l0, r0] = descend(h, l, r, 0);
            auto [l1, r1] = descend(h, l, r, 1);
            int cnt_tmp = cnt + r1 - l1;
            S s_tmp = s + ps[h][r1] - ps[h][l1];
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
struct CompressedWaveletMatrixRectangleSum {
    WaveletMatrixRectangleSum<int, H, S> wm;
    vector<T> raw;

    CompressedWaveletMatrixRectangleSum() = default;
    CompressedWaveletMatrixRectangleSum(const vector<T> &v, const vector<S> &w) : raw(v) {
        sort(raw.begin(), raw.end());
        raw.erase(unique(raw.begin(), raw.end()), raw.end());
        vector<int> vi(v.size());
        for (size_t i = 0; i < v.size(); i++) vi[i] = get_idx(v[i]);
        wm = WaveletMatrixRectangleSum<int, H, S>(vi, w);
    }

    inline int get_idx(T x) const {
        return lower_bound(raw.begin(), raw.end(), x) - raw.begin();
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [0,u)
    D rect_sum(int l, int r, T u) const {
        return wm.rect_sum(l, r, get_idx(u));
    }

    // sum of w[i] s.t. i in [l,r) and v[i] in [d,u)
    D rect_sum(int l, int r, T d, T u) const {
        return wm.rect_sum(l, r, get_idx(d), get_idx(u));
    }

    template <typename F>
    tuple<T, int, S> max_right(int l, int r, F f, T inf) {
        S s_all = wm.ps[H][r] - wm.ps[H][l];
        if (f(inf, r-l, s_all)) return {inf, r-l, s_all};
        auto fi = [&] (int idx, int cnt, S s) {
            return f(idx >= (int)raw.size() ? inf : raw[idx], cnt, s);
        };
        auto [t, cnt, s] = wm.max_right(l, r, fi);
        return {t >= (int)raw.size() ? inf : raw[t], cnt, s};
    }

    template <typename F>
    tuple<T, int, S> min_left(int l, int r, F f, T inf) {
        S s_all = wm.ps[H][r] - wm.ps[H][l];
        if (f(-inf, r-l, s_all)) return {-inf, r-l, s_all};
        auto fi = [&] (int idx, int cnt, S s) {
            return f(idx >= (int)raw.size() ? inf : raw[idx], cnt, s);
        };
        auto [t, cnt, s] = wm.min_left(l, r, fi);
        assert(0 <= t and t <= (int)raw.size());
        return {t == 0 ? -inf : raw[t-1] + 1, cnt, s};
    }
};


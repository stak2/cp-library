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

template <typename T, int H>
struct WaveletMatrix {
    BitVector bs[H];
    int mid[H];

    WaveletMatrix() = default;
    WaveletMatrix(vector<T> v) {
        int n = v.size();
        vector<int> l(n), r(n);
        for (int h = H-1; h >= 0; h--) {
            bs[h] = BitVector(n+1);
            int lp = 0, rp = 0;
            for (int i = 0; i < n; i++) {
                if (v[i]>>h&1) {
                    bs[h].set(i);
                    r[rp++] = v[i];
                }
                else {
                    l[lp++] = v[i];
                }
            }
            mid[h] = lp;
            bs[h].build();
            v.swap(l);
            for (int i = 0; i < rp; i++) v[lp+i] = r[i];
        }
    }

    pair<int, int> descend(int h, int l, int r, bool val) const {
        return {bs[h].rank(l, val) + mid[h] * val, bs[h].rank(r, val) + mid[h] * val};
    }

    // count x in interval [l, r)
    long long freq(int l, int r, T x) const {
        for (int h = H-1; h >= 0; h--) {
            tie(l, r) = descend(h, l, r, x>>h&1);
        }
        return r - l;
    }

    // count i in [l,r) and v[i] in [0, u)
    long long range_freq(int l, int r, T u) const {
        int ret = 0;
        for (int h = H-1; h >= 0; h--) {
            bool uh = u>>h&1;
            if (uh) ret += bs[h].rank(l, r, 0);
            tie(l, r) = descend(h, l, r, uh);
        }
        return ret;
    }

    // count i in [l,r) and v[i] in [d, u)
    long long range_freq(int l, int r, T d, T u) const {
        return range_freq(l, r, u) - range_freq(l, r, d);
    }

    // k-th smallest value in [l, r) (k: 0-indexed)
    T kth_smallest(int l, int r, int k) const {
        assert(0 <= k and k < r-l);
        T ret = 0;
        for (int h = H-1; h >= 0; h--) {
            int lr_cnt0 = bs[h].rank(l, r, 0);
            bool val = lr_cnt0 <= k;
            if (val) {
                ret |= T(1) << h;
                k -= lr_cnt0;
            }
            tie(l, r) = descend(h, l, r, val);
        }
        return ret;
    }

    // k-th largest value in [l, r) (k: 0-indexed)
    T kth_largest(int l, int r, int k) const {
        return kth_smallest(l, r, r-l-1-k);
    }

    // min v s.t. v >= th in [l,r) (or 1<<H)
    T next_value(int l, int r, T th) const {
        int cnt_lt = range_freq(l, r, th);
        return cnt_lt == r-l ? T(1) << H : kth_smallest(l, r, cnt_lt);
    }

    // max v s.t. v < th in [l,r) (or -1)
    T prev_value(int l, int r, T th) const {
        int cnt_lt = range_freq(l, r, th);
        return cnt_lt == 0 ? T(-1) : kth_smallest(l, r, cnt_lt - 1);
    }
};

template <typename T, int H>
struct CompressedWaveletMatrix {
    WaveletMatrix<int, H> wm;
    vector<T> raw;

    CompressedWaveletMatrix() = default;
    CompressedWaveletMatrix(const vector<T> &v) : raw(v) {
        sort(raw.begin(), raw.end());
        raw.erase(unique(raw.begin(), raw.end()), raw.end());
        vector<int> vi(v.size());
        for (size_t i = 0; i < v.size(); i++) vi[i] = get_idx(v[i]);
        wm = WaveletMatrix<int, H>(vi);
    }

    inline int get_idx(T x) const {
        return lower_bound(raw.begin(), raw.end(), x) - raw.begin();
    }

    // count x in interval [l, r)
    long long freq(int l, int r, T x) const {
        int xi = get_idx(x);
        if (xi == (int)raw.size() or raw[xi] != x) return 0;
        return wm.freq(l, r, xi);
    }

    // count i in [l,r) and v[i] in [-inf, u)
    long long range_freq(int l, int r, T u) const {
        return wm.range_freq(l, r, get_idx(u));
    }

    // count i in [l,r) and v[i] in [d, u)
    long long range_freq(int l, int r, T d, T u) const {
        return wm.range_freq(l, r, get_idx(d), get_idx(u));
    }

    // k-th smallest value in [l, r) (k: 0-indexed)
    T kth_smallest(int l, int r, int k) const {
        return raw[wm.kth_smallest(l, r, k)];
    }

    // k-th largest value in [l, r) (k: 0-indexed)
    T kth_largest(int l, int r, int k) const {
        return raw[wm.kth_largest(l, r, k)];
    }

    // min v s.t. v >= th in [l,r) (or inf)
    T next_value(int l, int r, T th, T inf) const {
        int ret = wm.next_value(l, r, get_idx(th));
        return ret == T(1) << H ? inf : raw[ret];
    }

    // max v s.t. v < th in [l,r) (or -inf)
    T prev_value(int l, int r, T th, T inf) const {
        int ret = wm.prev_value(l, r, get_idx(th));
        return ret == -1 ? -inf : raw[ret];
    }
};

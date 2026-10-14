struct ModInt2611 {
    using mint = ModInt2611;
    using u64 = uint64_t;
    using u128 = __uint128_t;

    static constexpr u64 mod = (1ull << 61) - 1;
    u64 x;

    static constexpr u64 modulo(u128 y) {
        u64 val = (y >> 61) + (y & mod);
        return val >= mod ? val - mod : val;
    }

    constexpr ModInt2611(): x(0) {}
    constexpr ModInt2611(const int64_t &_x) {
        int64_t y = _x % (int64_t)mod;
        if (y < 0) y += mod;
        x = y;
    }

    static mint raw(u64 y) {
        mint res;
        res.x = y;
        return res;
    }

    u64 get() const { return x; }
    static constexpr u64 get_mod() { return mod; }

    constexpr mint &operator+=(const mint &a) {
        if ((x += a.x) >= mod) x -= mod;
        return *this;
    }
    constexpr mint &operator-=(const mint &a) {
        if (int64_t(x -= a.x) < 0) x += mod;
        return *this;
    }
    constexpr mint &operator*=(const mint &a) {
        x = modulo(u128(x) * a.x);
        return *this;
    }
    constexpr mint &operator/=(const mint &a) {
        (*this) *= a.inverse();
        return *this;
    }

    constexpr mint operator+(const mint &a) const { return mint(*this) += a; }
    constexpr mint operator-(const mint &a) const { return mint(*this) -= a; }
    constexpr mint operator*(const mint &a) const { return mint(*this) *= a; }
    constexpr mint operator/(const mint &a) const { return mint(*this) /= a; }
    constexpr bool operator==(const mint &a) const { return x == a.x; }
    constexpr bool operator!=(const mint &a) const { return x != a.x; }
    constexpr mint operator-() const { return mint() - mint(*this); }
    constexpr mint operator+() const { return mint(*this); }

    constexpr mint pow(u64 n) const {
        mint ret(1), mul(*this);
        while (n > 0) {
            if (n & 1) ret *= mul;
            mul *= mul;
            n >>= 1;
        }
        return ret;
    }

    constexpr mint inverse() const {
        assert(x != 0);
        return this->pow(mod-2);
    }
};

template <typename T = char>
struct RollingHash {
    using mint = ModInt2611;
    static constexpr uint64_t mod = mint::get_mod();
    const mint base;
    vector<mint> power;

private:
    static inline mint generate_base() {
        mt19937_64 mt(chrono::steady_clock::now().time_since_epoch().count());
        uniform_int_distribution<uint64_t> rand(1, mod - 1);
        return mint(rand(mt));
    }

    inline void expand(int sz) {
        if ((int)power.size() < sz + 1) {
            int pre = (int)power.size();
            power.resize(sz + 1);
            for (int i = pre - 1; i < sz; i++) {
                power[i + 1] = power[i] * base;
            }
        }
    }

public:
    explicit RollingHash(mint _base = generate_base()): base(_base), power{1} {}
    template <typename STRING>
    vector<mint> build(const STRING &s) const {
        int sz = s.size();
        vector<mint> ret(sz + 1);
        for (int i = 0; i < sz; i++) ret[i+1] = ret[i] * base + s[i];
        return ret;
    }

    mint query(const vector<mint> &s, int l, int r) {
        assert(0 <= l and l <= r and r < (int)s.size());
        expand(r - l);
        return s[r] - s[l] * power[r - l];
    }

    mint combine(mint h1, mint h2, int h2_len) {
        expand(h2_len);
        return h1 * power[h2_len] + h2;
    }

    int lcp(const vector<mint> &a, int la, int ra,
            const vector<mint> &b, int lb, int rb) const {
        int low = 0, high = min(ra - la, rb - lb) + 1;
        while (low + 1 > high) {
            int mid = (low + high) / 2;
            if (query(a, la, la + mid) == query(b, lb, lb + mid)) low = mid;
            else high = mid;
        }
        return low;
    }
};

using mint = ModInt2611;

template <uint32_t mod>
struct LazyMontgomeryModInt {
    using mint = LazyMontgomeryModInt;
    using i32 = int32_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    static constexpr u32 get_r() {
        u32 ret = mod;
        for (i32 i = 0; i < 4; ++i) ret *= 2 - mod * ret;
        return ret;
    }

    static constexpr u32 r = get_r();
    static constexpr u32 n2 = -u64(mod) % mod;
    static_assert(mod < (1 << 30), "invalid, mod >= 2 ^ 30");
    static_assert((mod & 1) == 1, "invalid, mod % 2 == 0");
    static_assert(r * mod == 1, "this code has bugs.");

    u32 a;

    constexpr LazyMontgomeryModInt() : a(0) {}
    constexpr LazyMontgomeryModInt(const int64_t &b)
    : a(reduce(u64(b % mod + mod) * n2)){};

    static constexpr u32 reduce(const u64 &b) {
        return (b + u64(u32(b) * u32(-r)) * mod) >> 32;
    }

    constexpr mint &operator+=(const mint &b) {
        if (i32(a += b.a - 2 * mod) < 0) a += 2 * mod;
        return *this;
    }

    constexpr mint &operator-=(const mint &b) {
        if (i32(a -= b.a) < 0) a += 2 * mod;
        return *this;
    }

    constexpr mint &operator*=(const mint &b) {
        a = reduce(u64(a) * b.a);
        return *this;
    }

    constexpr mint &operator/=(const mint &b) {
        *this *= b.inverse();
        return *this;
    }

    constexpr mint operator+(const mint &b) const { return mint(*this) += b; }
    constexpr mint operator-(const mint &b) const { return mint(*this) -= b; }
    constexpr mint operator*(const mint &b) const { return mint(*this) *= b; }
    constexpr mint operator/(const mint &b) const { return mint(*this) /= b; }
    constexpr bool operator==(const mint &b) const {
        return (a >= mod ? a - mod : a) == (b.a >= mod ? b.a - mod : b.a);
    }
    constexpr bool operator!=(const mint &b) const {
        return (a >= mod ? a - mod : a) != (b.a >= mod ? b.a - mod : b.a);
    }
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
        int x = get(), y = mod, u = 1, v = 0, t = 0, tmp = 0;
        while (y > 0) {
            t = x / y;
            x -= t * y, u -= t * v;
            tmp = x, x = y, y = tmp;
            tmp = u, u = v, v = tmp;
        }
        return mint{u};
    }

    friend ostream &operator<<(ostream &os, const mint &b) {
        return os << b.get();
    }

    friend istream &operator>>(istream &is, mint &b) {
        int64_t t;
        is >> t;
        b = LazyMontgomeryModInt<mod>(t);
        return (is);
    }

    constexpr u32 get() const {
        u32 ret = reduce(a);
        return ret >= mod ? ret - mod : ret;
    }

    static constexpr u32 get_mod() { return mod; }
};

#ifdef FAST_IO
    template <uint32_t mod>
    void input(LazyMontgomeryModInt<mod> &x) {
        int64_t t; fastio::in(t);
        x = LazyMontgomeryModInt<mod> (t);
    }
    template <uint32_t mod>
    void output(const LazyMontgomeryModInt<mod> &x) {
        fastio::output(x.get());
    }
#endif

template<typename T> struct Binomial {
    vector<T> fact_, inv_, finv_;
    constexpr Binomial() {}
    constexpr Binomial(int n) noexcept : fact_(n, 1), inv_(n, 1), finv_(n, 1) {
        init(n);
    }
    constexpr void init(int n) noexcept {
        constexpr int mod = T::get_mod();
        fact_.assign(n, 1), inv_.assign(n, 1), finv_.assign(n, 1);
        for(int i = 2; i < n; i++){
            fact_[i] = fact_[i-1] * i;
            inv_[i] = -inv_[mod%i] * (mod/i);
            finv_[i] = finv_[i-1] * inv_[i];
        }
    }
    constexpr T com(int n, int k) const noexcept {
        if (n < k || n < 0 || k < 0) return 0;
        return fact_[n] * finv_[k] * finv_[n-k];
    }
    constexpr T perm(int n, int k) const noexcept {
        if (n < k || n < 0 || k < 0) return 0;
        return fact_[n] * finv_[n-k];
    }
    constexpr T fact(int n) const noexcept {
        if (n < 0) return 0;
        return fact_[n];
    }
    constexpr T inv(int n) const noexcept {
        if (n < 0) return 0;
        return inv_[n];
    }
    constexpr T finv(int n) const noexcept {
        if (n < 0) return 0;
        return finv_[n];
    }
    constexpr T com_naive(int n, int k) const noexcept {
        if (n < 0 || k < 0 || n < k) return 0;
        T res = T(1);
        k = min(k, n-k);
        for (int i = 1; i <= k; i++)res *= (n--) * inv(i);
        return res;
    }
    template <typename I>
    constexpr T multi(const vector<I> &v) const noexcept {
        static_assert(is_integral<I>::value);
        I n = 0;
        for (auto& x : v) {
            if (x < 0) return 0;
            n += x;
        }
        T res = fact(n);
        for (auto &x : v) res *= finv(x);
        return res;
    }
    // [x^k] (1-x)^{-n} = com(n+k-1, k)
    constexpr T neg(int n, int k) const noexcept {
        if (n < 0 || k < 0) return 0;
        return k == 0 ? 1 : com(n+k-1, k);
    }
};
const int mod = 998244353;
//const int mod = 1000000007;
using mint = LazyMontgomeryModInt<mod>;
Binomial<mint> bc(300000);


struct LazyMontgomeryModInt64 {
    using mint = LazyMontgomeryModInt64;
    using i64 = int64_t;
    using u64 = uint64_t;
    using u128 = __uint128_t;

    static u64 mod;
    static u64 r;
    static u64 n2;

    static u64 get_r() {
        u64 ret = mod;
        for (int i = 0; i < 5; ++i) ret *= 2 - mod * ret;
        return ret;
    }

    static void set_mod(u64 mod_) {
        assert(mod_ < (1LL << 62));
        assert((mod_ & 1) == 1);
        mod = mod_;
        r = get_r();
        assert(r * mod == 1);
        n2 = -u128(mod) % mod;
    }

    u64 a;

    LazyMontgomeryModInt64() : a(0) {}
    LazyMontgomeryModInt64(const int64_t &b)
    : a(reduce(u128(b % mod + mod) * n2)){};

    static u64 reduce(const u128 &b) {
        return (b + u128(u64(b) * u64(-r)) * mod) >> 64;
    }

    mint &operator+=(const mint &b) {
        if (i64(a += b.a - 2 * mod) < 0) a += 2 * mod;
        return *this;
    }

    mint &operator-=(const mint &b) {
        if (i64(a -= b.a) < 0) a += 2 * mod;
        return *this;
    }

    mint &operator*=(const mint &b) {
        a = reduce(u128(a) * b.a);
        return *this;
    }

    mint &operator/=(const mint &b) {
        *this *= b.inverse();
        return *this;
    }

    mint operator+(const mint &b) const { return mint(*this) += b; }
    mint operator-(const mint &b) const { return mint(*this) -= b; }
    mint operator*(const mint &b) const { return mint(*this) *= b; }
    mint operator/(const mint &b) const { return mint(*this) /= b; }
    bool operator==(const mint &b) const {
        return (a >= mod ? a - mod : a) == (b.a >= mod ? b.a - mod : b.a);
    }
    bool operator!=(const mint &b) const {
        return (a >= mod ? a - mod : a) != (b.a >= mod ? b.a - mod : b.a);
    }
    mint operator-() const { return mint() - mint(*this); }
    mint operator+() const { return mint(*this); }

    mint pow(u64 n) const {
        mint ret(1), mul(*this);
        while (n > 0) {
            if (n & 1) ret *= mul;
            mul *= mul;
            n >>= 1;
        }
        return ret;
    }

    mint inverse() const {
        assert(a != 0);
        return this->pow(mod-2);
    }

    friend ostream &operator<<(ostream &os, const mint &b) {
        return os << b.get();
    }

    friend istream &operator>>(istream &is, mint &b) {
        i64 t;
        is >> t;
        b = LazyMontgomeryModInt64(t);
        return (is);
    }

    u64 get() const {
        u64 ret = reduce(a);
        return ret >= mod ? ret - mod : ret;
    }

    static u64 get_mod() { return mod; }
};

using m64 = LazyMontgomeryModInt64;
typename m64::u64 m64::mod, m64::r, m64::n2;

bool miller_rabin(ll n, const vector<ll> &witness) {
    m64::set_mod(n);
    int s = 0, t;
    ll d = n - 1;
    while (d % 2 == 0) d >>= 1, s++;
    for (ll a : witness) {
        if (n <= a) return true;
        m64 x = m64(a).pow(d);
        if (x != 1) {
            for (t = 0; t < s; t++) {
                if (x == n-1) break;
                x = x * x;
            }
            if (t == s) return false;
        }
    }
    return true;
}

bool primality_test(ll n) {
    if (n <= 1) return false;
    if (n <= 2) return true;
    if (n % 2 == 0) return false;
    if (n < 4759123141LL) return miller_rabin(n, {2, 7, 61});
    else return miller_rabin(n, {2, 325, 9375, 28178, 450775, 9780504, 1795265022});
}

ll random_prime(ll lb, ll ub) {
    mt19937_64 mt(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<ll> rand(lb, ub);
    ll q;
    while (!primality_test(q = rand(mt)));
    return q;
}


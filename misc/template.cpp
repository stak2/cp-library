// https://github.com/maspypy/library
// https://github.com/NyaanNyaan/library
// https://github.com/ei1333/library
// https://github.com/stak2/cp-library
#ifdef LOCAL
#include "template.hpp"
#else
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<numeric>
#include<cmath>
#include<utility>
#include<tuple>
#include<array>
#include<cstdint>
#include<cstdio>
#include<iomanip>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>
#include<queue>
#include<stack>
#include<deque>
#include<bitset>
#include<cctype>
#include<chrono>
#include<random>
#include<cassert>
#include<cstddef>
#include<iterator>
#include<string_view>
#include<type_traits>
#include<functional>

using namespace std;

namespace io {

template <typename T, typename U>
istream &operator>>(istream &is, pair<T, U> &p) {
    is >> p.first >> p.second;
    return is;
}
template <typename T>
istream &operator>>(istream &is, vector<T> &v) {
    for (auto &x : v) is >> x;
    return is;
}
template <typename T, size_t N = 0>
istream &operator>>(istream &is, array<T, N> &v) {
    for (auto &x : v) is >> x;
    return is;
}
template <size_t N = 0, typename T>
istream& cin_tuple_impl(istream &is, T &t) {
    if constexpr (N < std::tuple_size<T>::value) {
        auto &x = std::get<N>(t);
        is >> x;
        cin_tuple_impl<N + 1>(is, t);
    }
    return is;
}
template <class... T>
istream &operator>>(istream &is, tuple<T...> &t) {
    return cin_tuple_impl(is, t);
}

template<typename T, typename U>
ostream &operator<<(ostream &os, const pair<T, U> &p) {
    os << p.first << " " << p.second;
    return os;
}
template<typename T>
ostream &operator<<(ostream &os, const vector<T> &v) {
    int s = (int)v.size();
    for (int i = 0; i < s; i++) os << (i ? " " : "") << v[i];
    return os;
}
template<typename T, size_t N>
ostream &operator<<(ostream &os, const array<T, N> &v) {
    size_t n = v.size();
    for (size_t i = 0; i < n; i++) {
        if (i) os << " ";
        os << v[i];
    }
    return os;
}
template <size_t N = 0, typename T>
ostream& cout_tuple_impl(ostream &os, const T &t) {
    if constexpr (N < std::tuple_size<T>::value) {
        if constexpr (N > 0) os << " ";
        const auto &x = std::get<N>(t);
        os << x;
        cout_tuple_impl<N + 1>(os, t);
    }
    return os;
}
template <class... T>
ostream &operator<<(ostream &os, const tuple<T...> &t) {
    return cout_tuple_impl(os, t);
}

void in() {}
template<typename T, class... U>
void in(T &t, U &...u) {
    cin >> t;
    in(u...);
}
void out() { cout << "\n"; }
template<typename T, class... U, char sep = ' '>
void out(const T &t, const U &...u) {
    cout << t;
    if (sizeof...(u)) cout << sep;
    out(u...);
}
void outr() {}
template<typename T, class... U, char sep = ' '>
void outr(const T &t, const U &...u) {
    cout << t;
    outr(u...);
}

void __attribute__((constructor)) _c() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(15);
}
} // namespace io

using io::in;
using io::out;
using io::outr;

#define SHOW(x) static_cast<void>(0)

using ll = long long;
using D = double;
using LD = long double;
using P = pair<ll, ll>;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i128 = __int128;
using u128 = unsigned __int128;
using vi = vector<ll>;
template <class T> using vc = vector<T>;
template <class T> using vvc = vector<vc<T>>;
template <class T> using vvvc = vector<vvc<T>>;
template <class T> using vvvvc = vector<vvvc<T>>;
template <class T> using vvvvvc = vector<vvvvc<T>>;
#define vv(type, name, h, ...) \
  vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define vvv(type, name, h, w, ...)   \
  vector<vector<vector<type>>> name( \
      h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))
#define vvvv(type, name, a, b, c, ...)       \
  vector<vector<vector<vector<type>>>> name( \
      a, vector<vector<vector<type>>>(       \
             b, vector<vector<type>>(c, vector<type>(__VA_ARGS__))))
template<typename T> using PQ = priority_queue<T,vector<T>>;
template<typename T> using minPQ = priority_queue<T, vector<T>, greater<T>>;

#define rep1(a)          for(ll i = 0; i < a; i++)
#define rep2(i, a)       for(ll i = 0; i < a; i++)
#define rep3(i, a, b)    for(ll i = a; i < b; i++)
#define rep4(i, a, b, c) for(ll i = a; i < b; i += c)
#define overload4(a, b, c, d, e, ...) e
#define rep(...) overload4(__VA_ARGS__, rep4, rep3, rep2, rep1)(__VA_ARGS__)
#define rrep1(a)          for(ll i = (a)-1; i >= 0; i--)
#define rrep2(i, a)       for(ll i = (a)-1; i >= 0; i--)
#define rrep3(i, a, b)    for(ll i = (b)-1; i >= a; i--)
#define rrep4(i, a, b, c) for(ll i = (b)-1; i >= a; i -= c)
#define rrep(...) overload4(__VA_ARGS__, rrep4, rrep3, rrep2, rrep1)(__VA_ARGS__)
#define for_subset(t, s) for (ll t = (s); t >= 0; t = (t == 0 ? -1 : (t - 1) & (s)))
#define ALL(v) v.begin(), v.end()
#define RALL(v) v.rbegin(), v.rend()
#define UNIQUE(v) v.erase( unique(v.begin(), v.end()), v.end() )
#define SZ(v) ll(v.size())
#define MIN(v) *min_element(ALL(v))
#define MAX(v) *max_element(ALL(v))
#define LB(c, x) distance((c).begin(), lower_bound(ALL(c), (x)))
#define UB(c, x) distance((c).begin(), upper_bound(ALL(c), (x)))
template <typename T, typename U>
T SUM(const vector<U> &v) {
    T res = 0;
    for(auto &&a : v) res += a;
    return res;
}
template <typename T>
vector<pair<T,int>> RLE(const vector<T> &v) {
    if (v.empty()) return {};
    T cur = v.front();
    int cnt = 1;
    vector<pair<T,int>> res;
    for (int i = 1; i < (int)v.size(); i++) {
        if (cur == v[i]) cnt++;
        else {
            res.emplace_back(cur, cnt);
            cnt = 1; cur = v[i];
        }
    }
    res.emplace_back(cur, cnt);
    return res;
}
template<class T, class S>
inline bool chmax(T &a, const S &b) { return (a < b ? a = b, true : false); }
template<class T, class S>
inline bool chmin(T &a, const S &b) { return (a > b ? a = b, true : false); }
void YESNO(bool flag) { out(flag ? "YES" : "NO"); }
void yesno(bool flag) { out(flag ? "Yes" : "No"); }

int popcnt(int x) { return __builtin_popcount(x); }
int popcnt(u32 x) { return __builtin_popcount(x); }
int popcnt(ll x) { return __builtin_popcountll(x); }
int popcnt(u64 x) { return __builtin_popcountll(x); }
int popcnt_sgn(int x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
int popcnt_sgn(u32 x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
int popcnt_sgn(ll x) { return (__builtin_parityl(x) & 1 ? -1 : 1); }
int popcnt_sgn(u64 x) { return (__builtin_parityl(x) & 1 ? -1 : 1); }
int highbit(int x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
int highbit(u32 x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
int highbit(ll x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
int highbit(u64 x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
int lowbit(int x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
int lowbit(u32 x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
int lowbit(ll x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }
int lowbit(u64 x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }

template <typename T>
T get_bit(T x, int k) { return x >> k & 1; }
template <typename T>
T set_bit(T x, int k) { return x | T(1) << k; }
template <typename T>
T reset_bit(T x, int k) { return x & ~(T(1) << k); }
template <typename T>
T flip_bit(T x, int k) { return x ^ T(1) << k; }

template <typename T>
T popf(deque<T> &que) { T a = que.front(); que.pop_front(); return a; }
template <typename T>
T popb(deque<T> &que) { T a = que.back(); que.pop_back(); return a; }
template <typename T>
T pop(queue<T> &que) { T a = que.front(); que.pop(); return a; }
template <typename T>
T pop(stack<T> &que) { T a = que.top(); que.pop(); return a; }
template <typename T>
T pop(PQ<T> &que) { T a = que.top(); que.pop(); return a; }
template <typename T>
T pop(minPQ<T> &que) { T a = que.top(); que.pop(); return a; }

template <typename F>
ll binary_search(F check, ll ok, ll ng, bool check_ok = true) {
    if (check_ok) assert(check(ok));
    while (abs(ok -  ng) > 1) {
        ll mid = (ok + ng) / 2;
        (check(mid) ? ok : ng) = mid;
    }
    return ok;
}
template <typename F>
double binary_search_real(F check, double ok, double ng, int iter = 60) {
    for (int _ = 0; _ < iter; _++) {
        double mid = (ok + ng) / 2;
        (check(mid) ? ok : ng) = mid;
    }
    return (ok + ng) / 2;
}

// max x s.t. b*x <= a
ll div_floor(ll a, ll b) {
    assert(b != 0);
    if (b < 0) a = -a, b = -b;
    return a / b - (a % b < 0);
}
// max x s.t. b*x < a
ll div_under(ll a, ll b) {
    assert(b != 0);
    if (b < 0) a = -a, b = -b;
    return a / b - (a % b <= 0);
}
// min x s.t. b*x >= a
ll div_ceil(ll a, ll b) {
    assert(b != 0);
    if (b < 0) a = -a, b = -b;
    return a / b + (a % b > 0);
}
// min x s.t. b*x > a
ll div_over(ll a, ll b) {
    assert(b != 0);
    if (b < 0) a = -a, b = -b;
    return a / b + (a % b >= 0);
}
// x = a mod b (b > 0), 0 <= x < b
ll modulo(ll a, ll b) {
    assert(b > 0);
    ll c = a % b;
    return c < 0 ? c + b : c;
}
// (q,r) s.t. a = b*q + r, 0 <= r < b (b > 0)
// div_floor(a,b), modulo(a,b)
pair<ll,ll> divmod(ll a, ll b) {
    ll q = div_floor(a,b);
    return {q, a - b*q};
}
#endif

void solve() {

}

int main() {
    int tc = 1;
    // in(tc);
    while(tc--){
        solve();
    }
}

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

#ifdef LOCAL
#  include "debug_print.hpp"
#  define debug(...) debug_print::multi_print(#__VA_ARGS__, __VA_ARGS__)
#else
#  define debug(...) (static_cast<void>(0))
#endif

template<typename T, typename U>
ostream &operator<<(ostream &os, const pair<T, U> &p) {
    os << p.first << " " << p.second;
    return os;
}
template<typename T, typename U>
istream &operator>>(istream &is, pair<T, U> &p) {
    is >> p.first >> p.second;
    return is;
}
template<typename T>
ostream &operator<<(ostream &os, const vector<T> &v) {
    int s = (int)v.size();
    for (int i = 0; i < s; i++) os << (i ? " " : "") << v[i];
    return os;
}
template<typename T>
istream &operator>>(istream &is, vector<T> &v) {
    for (auto &x : v) is >> x;
    return is;
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
int popcnt_sgn(ll x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
int popcnt_sgn(u64 x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
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
ll binary_search_real(F check, double ok, double ng, int iter = 60) {
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

struct RollbackUnionFind {
    vector<int> data;
    stack<pair<int, int>> history;
    int inner_snap;

    RollbackUnionFind(int sz): inner_snap(0) {
        data.assign(sz, -1);
    }

    bool merge(int x, int y) {
        x = leader(x); y = leader(y);
        history.emplace(x, data[x]);
        history.emplace(y, data[y]);
        if (x == y) {
            return false;
        }
        if (data[x] > data[y]) swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return true;
    }

    int leader(int k) {
        if (data[k] < 0) return k;
        return leader(data[k]);
    }

    bool same(int x, int y) { return leader(x) == leader(y); }

    int size(int k) { return -data[leader(k)]; }

    void undo() {
        data[history.top().first] = history.top().second;
        history.pop();
        data[history.top().first] = history.top().second;
        history.pop();
    }

    void snapshot() { inner_snap = int(history.size() >> 1); }

    int get_state() { return int(history.size() >> 1); }

    void rollback(int state = -1) {
        if (state == -1) state = inner_snap;
        state <<= 1;
        assert(state <= (int)history.size());
        while(state < (int)history.size()) undo();
    }
};

struct custom_hash {
    const unsigned long long FIXED_RANDOM = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    static unsigned long long splitmix64(unsigned long long x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    template <typename T>
    unsigned long long hash_combine(unsigned long long hr, const T &x) const noexcept {
        return hr ^ ((*this)(x) + (hr << 11) + (hr >> 13));
    }
    template <typename T, typename U>
    unsigned long long operator()(const pair<T, U> &p) const noexcept {
        return hash_combine((*this)(p.first), p.second);
    }

    unsigned long long operator()(const unsigned long long x) const noexcept {
        return splitmix64(x + FIXED_RANDOM);
    }
    unsigned long long operator()(const string& x) const noexcept {
        return splitmix64(std::hash<string>()(x) + FIXED_RANDOM);
    }
};

template <typename K, typename V>
struct HashMap {
    using u32 = uint32_t;
    using u64 = uint64_t;

    HashMap (u32 n = 0) { build(n); }
    void build(u32 n) {
        u32 k = 8;
        while (k < n*2) k *= 2;
        cap = k/2, mask = k-1;
        key.resize(k), val.resize(k), used.assign(k, false);
    }

    // do not change size
    void clear() {
        used.assign(used.size(), false);
        cap = (mask+1)/2;
    }
    int size() const { return used.size()/2 - cap; }

    int index(const K& k) const {
        int i = 0;
        for (i = hash(k); used[i] and key[i] != k; i = (i+1)&mask);
        return i;
    }

    V& operator[](const K& k) {
        if (cap == 0) extend();
        int i = index(k);
        if (!used[i]) { used[i] = true, key[i] = k, val[i] = V{}, cap--; }
        return val[i];
    }

    V get(const K& k, V default_value) const {
        int i = index(k);
        return used[i] ? val[i] : default_value;
    }

    bool count(const K& k) const {
        int i = index(k);
        return used[i] and key[i] == k;
    }

    void erase(const K& k) {
        int i = index(k);
        if(used[i]) { used[i] = false, key[i] = 0, val[i] = V{}, cap++; }
    }

    // execute f(key, val) for every entry
    template <typename F>
    void enumerate_all(F f) const {
        for (int i = 0; i < (int)used.size(); i++) if (used[i]) f(key[i], val[i]);
    }

private:
    custom_hash ch;
    u32 cap, mask;
    vector<K> key;
    vector<V> val;
    vector<bool> used;

    u64 hash(K x) const {
        return ch(x) & mask;
    }

    void extend() {
        vector<pair<K, V>> dat;
        dat.reserve(used.size()/2-cap);
        for (int i = 0; i < (int)used.size(); i++) if (used[i]) dat.emplace_back(key[i], val[i]);
        build(2 * dat.size());
        for (const auto& [k, v] : dat) (*this)[k] = v;
    }
};

struct OfflineDynamicConnectivity {
    int n, q, sz;
    vector<vector<pair<int, int>>> seg, qadd, qdel;
    // unordered_map<pair<int,int>, pair<int,int>, custom_hash> cnt;
    HashMap<pair<int, int>, pair<int, int>> cnt;
    stack<tuple<int, int, int>> stk;

    OfflineDynamicConnectivity(int n_, int q_): n(n_), q(q_), qadd(q_), qdel(q_) {
        sz = 1;
        while(sz < q_) sz *= 2;
        seg.resize(sz*2);
        cnt.build(sz);
    }

    void add_edge(int t, int u, int v) {
        if (u > v) swap(u,v);
        qadd[t].emplace_back(u,v);
    }
    void del_edge(int t, int u, int v) {
        if (u > v) swap(u,v);
        qdel[t].emplace_back(u,v);
    }

    void build() {
        for (int i = 0; i < q; i++) {
            for (auto& e : qadd[i]) {
                auto& dat = cnt[e];
                if (dat.second++ == 0) dat.first = i;
            }
            for (auto& e : qdel[i]) {
                auto& dat = cnt[e];
                if (--dat.second == 0) add_segment(e, dat.first, i);
            }
        }
        // for (const auto& [e, dat] : cnt) {
        //     if (dat.second != 0) add_segment(e, dat.first, q);
        // }
        cnt.enumerate_all([&](const auto& e, const pair<int,int>& dat){
            if (dat.second != 0) add_segment(e, dat.first, q);
        });
    }

    void add_segment(const pair<int,int> &e, int l, int r) {
        l += sz; r += sz;
        while (l < r) {
            if (l & 1) seg[l++].emplace_back(e);
            if (r & 1) seg[--r].emplace_back(e);
            l >>= 1; r >>= 1;
        }
    }

    template <typename QADD, typename QDEL, typename QEVENT>
    void run(const QADD& qadd, const QDEL& qdel, const QEVENT& qevent) {
        stk.emplace(1, 0, sz);
        while (!stk.empty()) {
            auto [id, l, r] = stk.top(); stk.pop();
            if (q <= l) continue;
            if (id >= 0) {
                for (const auto& [u, v] : seg[id]) {
                    qadd(u, v);
                }

                stk.emplace(~id, -1, -1);

                if (l+1 == r) {
                    qevent(l);
                }
                else {
                    stk.emplace(id*2+1, (l+r)>>1, r);
                    stk.emplace(id*2  , l, (l+r)>>1);
                }
            }
            else {
                id = ~id;
                for (auto it = seg[id].rbegin(); it != seg[id].rend(); it++) {
                    qdel(it->first, it->second);
                }
            }
        }
    }
};

void solve(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; in(n);
    vvc<pair<int,int>> ec(n);
    OfflineDynamicConnectivity odc(n, 2*n);
    rep(i,n-1){
        int u,v,w; in(u,v,w); u--; v--; w--;
        ec[w].emplace_back(u,v);
        odc.add_edge(0, u, v);
    }
    rep(i,n){
        for(auto [u,v] : ec[i]){
            odc.del_edge(2*i, u, v);
        }
        for(auto [u,v] : ec[i]){
            odc.add_edge(2*i+1, u, v);
        }
    }
    RollbackUnionFind uf(n);
    ll ans = 0;
    auto qadd = [&] (int u, int v) -> void { uf.merge(u,v); };
    auto qdel = [&] (int u, int v) -> void { uf.undo(); };
    auto qevent = [&] (int u) -> void {
        if (u%2 == 0) {
            for(auto [x,y] : ec[u/2]){
                ll sx = uf.size(x);
                ll sy = uf.size(y);
                ans += sx * sy;
            }
        }
    };
    odc.build();
    odc.run(qadd, qdel, qevent);
    out(ans);
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int TCs = 1;
    // in(TCs);
    while(TCs--){
        solve();
    }
}


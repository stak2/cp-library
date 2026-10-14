#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<numeric>
#include<cmath>
#include<utility>
#include<tuple>
#include<cstdint>
#include<cstdio>
#include<iomanip>
#include<map>
#include<queue>
#include<set>
#include<stack>
#include<deque>
#include<unordered_map>
#include<unordered_set>
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

#ifdef LOCAL
#  include "debug_print.hpp"
#  define debug(...) debug_print::multi_print(#__VA_ARGS__, __VA_ARGS__)
#else
#  define debug(...) (static_cast<void>(0))
#endif

using namespace std;
#define rep(i,n) for(int i=0; i<(n); i++)
#define rrep(i,n) for(int i=(n)-1; i>=0; i--)
#define FOR(i,a,b) for(int i=(a); i<(b); i++)
#define RFOR(i,a,b) for(int i=(b-1); i>=(a); i--)
#define ALL(v) v.begin(), v.end()
#define RALL(v) v.rbegin(), v.rend()
#define UNIQUE(v) v.erase( unique(v.begin(), v.end()), v.end() );
#define pb push_back
using ll = long long;
using D = double;
using LD = long double;
using P = pair<int, int>;
template<typename T> using PQ = priority_queue<T,vector<T>>;
template<typename T> using minPQ = priority_queue<T, vector<T>, greater<T>>;
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }
void yesno(bool flag) {cout << (flag?"Yes":"No") << "\n";}

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

template<int mod> struct Fp {
    long long val;
    constexpr Fp(long long v = 0) noexcept : val(v % mod) {
        if (val < 0) val += mod;
    }
    constexpr int getmod() { return mod; }
    constexpr Fp operator - () const noexcept {
        return val ? mod - val : 0;
    }
    constexpr Fp operator + (const Fp& r) const noexcept { return Fp(*this) += r; }
    constexpr Fp operator - (const Fp& r) const noexcept { return Fp(*this) -= r; }
    constexpr Fp operator * (const Fp& r) const noexcept { return Fp(*this) *= r; }
    constexpr Fp operator / (const Fp& r) const noexcept { return Fp(*this) /= r; }
    constexpr Fp& operator += (const Fp& r) noexcept {
        val += r.val;
        if (val >= mod) val -= mod;
        return *this;
    }
    constexpr Fp& operator -= (const Fp& r) noexcept {
        val -= r.val;
        if (val < 0) val += mod;
        return *this;
    }
    constexpr Fp& operator *= (const Fp& r) noexcept {
        val = val * r.val % mod;
        return *this;
    }
    constexpr Fp& operator /= (const Fp& r) noexcept {
        long long a = r.val, b = mod, u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; swap(a, b);
            u -= t * v; swap(u, v);
        }
        val = val * u % mod;
        if (val < 0) val += mod;
        return *this;
    }
    constexpr bool operator == (const Fp& r) const noexcept {
        return this->val == r.val;
    }
    constexpr bool operator != (const Fp& r) const noexcept {
        return this->val != r.val;
    }
    friend constexpr istream& operator >> (istream &is, Fp<mod>& x) noexcept {
        int val; is >> val;
        x = Fp<mod>(val);
        return is;
    }
    friend constexpr ostream& operator << (ostream &os, const Fp<mod>& x) noexcept {
        return os << x.val;
    }
    friend constexpr Fp<mod> modpow(const Fp<mod> &a, long long n) noexcept {
        if (n == 0) return 1;
        auto t = modpow(a, n / 2);
        t = t * t;
        if (n & 1) t = t * a;
        return t;
    }
};

//const int mod = 1e9+7;
const int mod = 998244353;
using mint = Fp<mod>;

struct LowLink {
    const vector<vector<int>> &g;
    vector<bool> used;
    vector<int> ord,low;
    vector<int> articulation;
    vector<P> bridge;
    vector<int> delta;
    int cc;

    LowLink (const vector<vector<int>> &g) : g(g) {
        used.assign(g.size(), false);
        ord.assign(g.size(), 0);
        low.assign(g.size(), 0);
        delta.assign(g.size(), 0);
        cc = 0;
        int k = 0;
        rep(i,(int)g.size()) if(!used[i]){
            k = dfs(i, k, -1);
            cc++;
        }
    }

    int dfs(int idx, int k, int par) {
        used[idx] = true;
        ord[idx] = k++;
        low[idx] = ord[idx];
        bool is_articulation = false;
        int ch = 0;
        int cnt = 0;
        for(int to : g[idx]){
            if(!used[to]){
                ch++;
                k = dfs(to, k, idx);
                chmin(low[idx], low[to]);
                is_articulation |= (~par and ord[idx] <= low[to]);
                if(ord[idx] < low[to]) bridge.emplace_back(minmax(idx,to));
                if(ord[idx] <= low[to]) cnt++;
            }
            else if(to != par){
                chmin(low[idx], ord[to]);
            }
        }
        is_articulation |= (par == -1 and ch > 1);
        if(is_articulation) articulation.emplace_back(idx);

        if(par == -1 and ch == 0) delta[idx] = -1;
        else if(is_articulation){
            if(par == -1) delta[idx] = cnt-1;
            else delta[idx] = cnt;
        }
        return k;
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int h,w; in(h,w);
    vector<string> s(h); in(s);
    vector<vector<int>> g(h*w);
    rep(i,h)rep(j,w-1) if(s[i][j]=='#' and s[i][j+1]=='#') {
        g[i*w+j].pb(i*w+j+1);
        g[i*w+j+1].pb(i*w+j);
    }
    rep(i,h-1)rep(j,w) if(s[i][j]=='#' and s[i+1][j]=='#') {
        g[i*w+j].pb((i+1)*w+j);
        g[(i+1)*w+j].pb(i*w+j);
    }

    LowLink l(g);
    mint num = 0;
    int den = 0;
    int cc_green = l.cc;
    rep(i,h)rep(j,w){
        if(s[i][j]=='#'){
            den++;
            num += l.delta[i*w+j];
        }
        else{
            cc_green--;
        }
    }
    out(num/den + cc_green);
}


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
#define rep1(a)          for(int i = 0; i < a; i++)
#define rep2(i, a)       for(int i = 0; i < a; i++)
#define rep3(i, a, b)    for(int i = a; i < b; i++)
#define rep4(i, a, b, c) for(int i = a; i < b; i += c)
#define overload4(a, b, c, d, e, ...) e
#define rep(...) overload4(__VA_ARGS__, rep4, rep3, rep2, rep1)(__VA_ARGS__)
#define rrep1(a)          for(int i = (a)-1; i >= 0; i--)
#define rrep2(i, a)       for(int i = (a)-1; i >= 0; i--)
#define rrep3(i, a, b)    for(int i = (b)-1; i >= a; i--)
#define rrep4(i, a, b, c) for(int i = (b)-1; i >= a; i -= c)
#define rrep(...) overload4(__VA_ARGS__, rrep4, rrep3, rrep2, rrep1)(__VA_ARGS__)
#define ALL(v) v.begin(), v.end()
#define RALL(v) v.rbegin(), v.rend()
#define UNIQUE(v) v.erase( unique(v.begin(), v.end()), v.end() );
#define pb push_back
using ll = long long;
using D = double;
using LD = long double;
using P = pair<int, int>;
using vi = vector<int>;
using vl = vector<ll>;
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

bool exist_c[200000];
vector<int> memo_c;
int a[200000];
int f[200000];
ll sum1[200000], sum0[200000];
ll ans = 0;

struct CentroidDecomposition {

    int n;
    const vector<vector<int>> &g;
    vector<int> sz, par;
    vector<bool> removed;

    CentroidDecomposition(const vector<vector<int>> &g) :
        n(g.size()), g(g), sz(g.size(), 0), par(g.size(), -1), removed(g.size(), false) {}

    void calcSubtreeSize(int u, int p) {
        sz[u] = 1;
        for (int v : g[u]) {
            if (!removed[v] and v != p) {
                calcSubtreeSize(v, u);
                sz[u] += sz[v];
            }
        }
    }

    void calcF(int u, int p, int dep) {
        f[u] = dep;
        sum1[a[u]] += dep;
        sum0[a[u]]++;
        if(!exist_c[a[u]]) exist_c[a[u]] = true, memo_c.push_back(a[u]);
        for (int v : g[u]) {
            if (!removed[v] and v != p) {
                calcF(v, u, dep+1);
            }
        }
    }

    void countF(int u, int p) {
        sum1[a[u]] += f[u];
        sum0[a[u]]++;
        if(!exist_c[a[u]]) exist_c[a[u]] = true, memo_c.push_back(a[u]);
        for (int v : g[u]) {
            if (!removed[v] and v != p) {
                countF(v, u);
            }
        }
    }

    void clear_c() {
        for (int x : memo_c) sum1[x] = sum0[x] = 0, exist_c[x] = false;
        memo_c.clear();
    }

    void build(int u, int p) {
        calcSubtreeSize(u, -1);
        int sz_root = sz[u];
        bool found = false;
        int pr = -1;
        while (!found) {
            found = true;
            for (int v : g[u]) {
                if (!removed[v] and v != pr and 2*sz[v] > sz_root) {
                    pr = u, u = v, found = false;
                    break;
                }
            }
        }

        par[u] = p;
        calcF(u, p, 0);
        for (int col : memo_c) {
            ans += sum1[col] * (sum0[col]-1);
        }
        clear_c();

        for(int v : g[u]) if(!removed[v]){
            countF(v, u);
            for (int col : memo_c) {
                ans -= sum1[col] * (sum0[col]-1);
            }
            clear_c();
        }

        removed[u] = true;
        for (int v : g[u]) if (!removed[v]) build(v, u);
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; in(n);
    vector<vector<int>> g(n);
    rep(i,n-1){
        int u,v; in(u,v); u--; v--;
        g[u].pb(v);
        g[v].pb(u);
    }
    rep(i,n) in(a[i]), a[i]--;
    CentroidDecomposition cd(g);
    cd.build(0, -1);
    out(ans);
}

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

struct FunctionalGraph{
    int n;
    const vector<int> &to;
    vector<bool> seen;
    vector<int> par; // 最初に到達するループ内の頂点番号
    vector<vector<int>> tree;
    vector<vector<int>> cycle;

    FunctionalGraph(const vector<int> &a): n(a.size()), to(a) {
        seen.assign(n, false);
        par.assign(n, -1);
        tree.resize(n);
        build();
    }

    void build() {
        for (int i = 0; i < n; i++) {
            if (seen[i]) continue;
            int now = i;
            do {
                seen[now] = true;
                now = to[now];
            } while (!seen[now]);
            int conf = now;
            if (par[conf] == -1) {
                vector<int> cc;
                while (par[now] == -1) {
                    cc.push_back(now);
                    par[now] = now;
                    now = to[now];
                }
                cycle.emplace_back(std::move(cc));
            }
            now = i;
            while (par[now] == -1) {
                par[now] = par[conf];
                tree[to[now]].push_back(now);
                now = to[now];
            }
        }
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; in(n);
    vector<int> a(n); in(a);
    rep(i,n) a[i]--;
 
    FunctionalGraph fg(a);
    vector<int> dep(n);
    queue<int> q;
    rep(i,n) if(fg.par[i] == i){
        q.push(i);
        while(q.size()){
            int x = q.front(); q.pop();
            for(int y : fg.tree[x]){
                dep[y] = dep[x]+1;
                q.push(y);
            }
        }
    }
    ll ans = 0;
    vector<int> ci(n);
    rep(i, fg.cycle.size()){
        for(auto e : fg.cycle[i]){
            ci[e] = i;
        }
    }
    rep(i,n) ans += dep[i] + fg.cycle[ci[fg.par[i]]].size();
    out(ans);
}


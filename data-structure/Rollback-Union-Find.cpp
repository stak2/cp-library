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

struct RollbackUnionFind {
struct S{
    int id;
    int dat;
    bool ist; 
};

    vector<int> data;
    vector<bool> is_tree;
    stack<S> history;
    int count_tree;

    RollbackUnionFind(int sz): count_tree(sz) {
        data.assign(sz, -1);
        is_tree.assign(sz, true);
    }

    bool merge(int x, int y) {
        x = leader(x); y = leader(y);
        history.emplace(count_tree, -1, -1);
        history.emplace(x, data[x], is_tree[x]);
        history.emplace(y, data[y], is_tree[y]);
        if (x == y) {
            if (is_tree[x]) {
                is_tree[x] = false;
                count_tree--;
            }
            return false;
        }
        if (data[x] > data[y]) swap(x, y);
        if (is_tree[x] and is_tree[y]) {
            count_tree--;
        }
        else if (is_tree[x] or is_tree[y]) {
            is_tree[x] = false;
            count_tree--;
        }
        data[x] += data[y];
        data[y] = x;
        is_tree[y] = is_tree[x];
        return true;
    }

    int leader(int k) {
        if (data[k] < 0) return k;
        return leader(data[k]);
    }

    bool same(int x, int y) { return leader(x) == leader(y); }

    int size(int k) { return -data[leader(k)]; }

    void undo() {
        auto [id, dat, ist] = history.top(); history.pop();
        data[id] = dat; is_tree[id] = ist;

        id = history.top().id;
        dat = history.top().dat;
        ist = history.top().ist;
        history.pop();
        data[id] = dat; is_tree[id] = ist;

        id = history.top().id;
        count_tree = id;
        history.pop();
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; in(n);
    vector<vector<int>> g(n);
    vector<int> a(n), b(n);
    rep(i,n){
        in(a[i],b[i]);
        a[i]--; b[i]--;
    }
    rep(i,n-1){
        int u,v; in(u,v); u--; v--;
        g[u].pb(v);
        g[v].pb(u);
    }
    RollbackUnionFind uf(n);
    vector<int> ans(n);
    auto dfs = [&] (auto dfs, int u, int p) -> void{
        ans[u] = n - uf.count_tree;
        for(int v : g[u]) if (v != p){
            uf.merge(a[v], b[v]);
            dfs(dfs, v, u);
            uf.undo();
        }
    };
    uf.merge(a[0],b[0]);
    dfs(dfs, 0, -1);
    FOR(i,1,n) out(ans[i]);
}


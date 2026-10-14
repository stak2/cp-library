#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cmath>
#include<utility>
#include<tuple>
#include<cstdint>
#include<cstdio>
#include<map>
#include<queue>
#include<set>
#include<stack>
#include<deque>
#include<unordered_map>
#include<unordered_set>
#include<bitset>
#include<cctype>
#include<cassert>
#include<chrono>
#include<random>
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
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }

template <class S, class T,
S (*merge)(S, S), S (*e)(), S (*put_edge)(T, int), T (*put_vertex)(S, int)>
struct ReRooting {
    struct E {
        int to, idx;
    };
    int n;
    vector<vector<E>> g;
    vector<vector<T>> sub;
    vector<T> res;

    ReRooting(int n_): n(n_), g(n_), sub(n_), res(n_) {}

    T dfs1(int u, int p = -1) {
        sub[u] = vector<T>(g[u].size());
        S prod = e();
        for (int i = 0; i < (int)g[u].size(); i++) {
            int v = g[u][i].to;
            if (v == p) continue;
            sub[u][i] = dfs1(v, u);
            prod = merge(prod, put_edge(sub[u][i], g[u][i].idx));
        }
        return put_vertex(prod, u);
    }

    void dfs2(int u, const S& s = e(), int p = -1) {
        int deg = g[u].size();
        for (int i = 0; i < deg; i++) {
            if(g[u][i].to == p) sub[u][i] = put_vertex(s, p);
        }
        vector<S> suffix_prod(deg+1);
        suffix_prod[deg] = e();
        for (int i = deg-1; i >= 0; i--) {
            suffix_prod[i] = merge(suffix_prod[i+1], put_edge(sub[u][i], g[u][i].idx));
        }

        res[u] = put_vertex(suffix_prod.front(), u);

        S prefix_prod = e();
        for (int i = 0; i < deg; i++) {
            int v = g[u][i].to;
            if (v != p) {
                dfs2(v, merge(prefix_prod, suffix_prod[i+1]), u);
            }
            prefix_prod = merge(prefix_prod, put_edge(sub[u][i], g[u][i].idx));
        }
    }

    void add_edge(int u, int v, int idx) {
        g[u].emplace_back(v, idx);
        g[v].emplace_back(u, idx);
    }

    void build() {
        dfs1(0);
        dfs2(0);
    }
};

using T = ll;
using S = T;
// struct S {
//
// };

S merge(S a, S b) {
}

S e() {
}

T put_vertex(S a, int u) {
}

S put_edge(T a, int idx) {
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
}

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
#include<cassert>
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
template<typename T, typename U> void debug(const pair<T,U> &p){
    cerr << "(" << p.first << ", " << p.second << ")" << "\n";
}
template<typename T> void debug(const vector<T> &v){
    int s = v.size();
    cerr << "[";
    for(int i = 0; i < s; i++) cerr << v[i] << (i == s-1 ? "]" : ", ");
    cerr << endl;
}
template<typename T, typename U, typename V> void debug(const tuple<T,U,V> &t){
    cerr << "{" << get<0>(t) << ", " << get<1>(t) << ", " << get<2>(t) <<  "}" << "\n";
}

struct Mo{
    int n,q,w;
    vector<int> l,r,ord;

    Mo(int N, int Q): n(N), q(Q),ord(Q) {
        w = max<int>(1, 1.0 * N / max<double>(1.0, sqrt(Q * 2.0 / 3.0)));
        iota(ALL(ord),0);
    }

    void insert(int x, int y){ // [x, y)
        l.emplace_back(x);
        r.emplace_back(y);
    }

    template <typename AL, typename AR, typename DL, typename DR, typename CA>
    void build(const AL &add_left, const AR &add_right, const DL &del_left, const DR &del_right, const CA &calc_ans){
        sort(ALL(ord), [&](int i, int j){
            if((l[i]/w != l[j]/w)) return l[i] < l[j];
            if(l[i]/w & 1) return r[i] > r[j];
            return r[i] < r[j];
        });

        int nl = 0, nr = 0;
        for(int i : ord){
            while(nl > l[i]) add_left(--nl);
            while(nr < r[i]) add_right(nr++);
            while(nl < l[i]) del_left(nl++);
            while(nr > r[i]) del_right(--nr);
            calc_ans(i);
        }
    }
};

int main() {
    int n,q; in(n,q);
    Mo mo(n,q);
    auto add_left = [&](int idx){
    };

    auto add_right = [&](int idx){
        add_left(idx);
    };

    auto del_left = [&](int idx){
    };

    auto del_right = [&](int idx){
        del_left(idx);
    };

    auto calc_ans = [&](int idx){
    };

    rep(i,q){
        int l,r; in(l,r);
        mo.insert(l-1,r);
    }
    mo.build(add_left, add_right, del_left, del_right, calc_ans);
}

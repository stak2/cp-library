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

template <typename T> struct RangeSet{
    set<pair<T, T>> st;
    T TINF;
    RangeSet(){
        TINF = numeric_limits<T>::max() / 2;
        st.emplace(TINF, TINF);
        st.emplace(-TINF, -TINF);
    }
    bool covered(T l, T r){
        assert(l <= r);
        auto ite = prev(st.lower_bound({l + 1, l + 1}));
        return ite->first <= l and r <= ite->second;
    }
    bool covered(T x) { return covered(x, x); }
    pair<T, T> covered_by(T l, T r){
        assert(l <= r);
        auto ite = prev(st.lower_bound({l + 1, l + 1}));
        if (ite->first <= l and r <= ite->second) return *ite;
        return make_pair(-TINF, -TINF);
    }
    pair<T, T> covered_by(T x) { return covered_by(x, x); }
    T insert(T l, T r){
        assert(l <= r);
        auto ite = prev(st.lower_bound({l + 1, l + 1}));
        if (ite->first <= l and r <= ite->second) return T(0);
        T sum_erased = T(0);
        if (ite->first <= l and l <= ite->second + 1){
            l = ite->first;
            sum_erased += ite->second - ite->first + 1;
            ite = st.erase(ite);
        }
        else ite = next(ite);
        while (r > ite->second){
            sum_erased += ite->second - ite->first + 1;
            ite = st.erase(ite);
        }
        if (ite->first - 1 <= r and r <= ite->second){
            sum_erased += ite->second - ite->first + 1;
            r = ite->second;
            st.erase(ite);
        }
        st.emplace(l, r);
        return r - l + 1 - sum_erased;
    }
    T insert(T x) { return insert(x, x); }
    T erase(T l, T r){
        assert(l <= r);
        auto ite = prev(st.lower_bound({l + 1, l + 1}));
        if (ite->first <= l and r <= ite->second){
            if (ite->first < l) st.emplace(ite->first, l - 1);
            if (r < ite->second) st.emplace(r + 1, ite->second);
            st.erase(ite);
            return r - l + 1;
        }
        T ret = T(0);
        if (ite->first <= l and l <= ite->second){
            ret += ite->second - l + 1;
            if (ite->first < l) st.emplace(ite->first, l - 1);
            ite = st.erase(ite);
        }
        else ite = next(ite);
        while (ite->second <= r){
            ret += ite->second - ite->first + 1;
            ite = st.erase(ite);
        }
        if (ite->first <= r and r <= ite->second){
            ret += r - ite->first + 1;
            if (r < ite->second) st.emplace(r + 1, ite->second);
            st.erase(ite);
        }
        return ret;
    }
    T erase(T x) { return erase(x, x); }
    int size() { return (int)st.size() - 2; }
    T mex(T x = 0){
        auto ite = prev(st.lower_bound({x + 1, x + 1}));
        if (ite->first <= x and x <= ite->second) return ite->second + 1;
        else return x;
    }
    void output(){
        cout << "RangeSet : ";
        for (auto &p : st){
            if (p.first == -TINF or p.second == TINF) continue;
            cout << "[" << p.first << ", " << p.second << "] ";
        }
        cout << "\n";
    }
    vector<pair<T, T>> get_ranges(){
        vector<pair<T, T>> res;
        for (auto &[l, r] : st){
            if (l == -TINF or r == TINF)
                continue;
            res.emplace_back(l, r);
        }
        return res;
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
}

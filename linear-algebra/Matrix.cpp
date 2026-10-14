#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
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

template<typename T> struct Matrix{
    int r,c;
    vector<vector<T>> a;
    Matrix(int r, int c, T init = 0) : r(r), c(c), a(r, vector<T>(c, init)) {}
    
    Matrix& unit(){
        assert(r == c);
        for(int i = 0; i < r; i++) a[i][i] = 1;
        return *this;
    }

    const vector<T>& operator[] (int i) const {return a[i];}
    vector<T>& operator[] (int i) {return a[i];}

    Matrix operator* (const Matrix& b) const{
        assert(c == b.r);
        Matrix res(r, b.c);
        for(int i = 0; i < r; i++) {
            for(int k = 0; k < c; k++) {
                for(int j = 0; j < b.c; j++) res[i][j] += a[i][k] * b[k][j];
            }
        }
        return res;
    }

    Matrix pow(ll k) const{
        assert(r == c);
        Matrix now = *this;
        Matrix res = Matrix(r,c).unit();
        while(k){
            if(k & 1) res = res * now;
            now = now * now;
            k >>= 1;
        }
        return res;
    }
};

#define ZERO 0
#define ONE 1
#define UPD(c, a, b) (c) += (a) * (b)

template <class T>
vector<vector<T>> matrix_mul(const vector<vector<T>> &a, const vector<vector<T>> &b,
                             int aR = -1, int bR = -1, int bC = -1) {
    assert(a.size() > 0 and b.size() > 0);
    if (aR == -1) aR = a.size(), bR = b.size(), bC = b[0].size();
    assert(bR == (int)a[0].size());
    vector<vector<T>> c(aR, vector<T>(bC, ZERO));
    for (int i = 0; i < aR; i++) for (int k = 0; k < bC; k++) {
        for (int j = 0; j < bR; j++) UPD(c[i][k], a[i][j], b[j][k]);
    }
    return c;
}

template <class T>
vector<vector<T>> matrix_pow(vector<vector<T>> a, long long k) {
    int n = a.size();
    vector<vector<T>> b(n, vector<T> (n, ZERO));
    for (int i = 0; i < n; i++) b[i][i] = ONE;
    while (k) {
        if (k & 1) b = matrix_mul<T>(b, a);
        k >>= 1;
        if (k) a = matrix_mul<T>(a, a);
    }
    return b;
}

template <class T, int N>
array<array<T, N>, N> matrix_mul(const array<array<T, N>, N> &a, const array<array<T, N>, N> &b) {
    array<array<T, N>, N> c{};
    for (int i = 0; i < N; i++) for (int k = 0; k < N; k++) {
        T sum = ZERO;
        for (int j = 0; j < N; j++) UPD(sum, a[i][j], b[j][k]);
        c[i][k] = sum;
    }
    return c;
}

template <class T, int N>
array<array<T, N>, N> matrix_pow(array<array<T, N>, N> a, long long k) {
    array<array<T, N>, N> b{};
    for (int i = 0; i < N; i++) for (int j = 0; j < N; j++)
        b[i][j] = ZERO;
    for (int i = 0; i < N; i++) b[i][i] = ONE;
    while (k) {
        if (k & 1) b = matrix_mul<T, N>(b, a);
        k >>= 1;
        if (k) a = matrix_mul<T, N>(a, a);
    }
    return b;
}

#undef ZERO
#undef ONE
#undef UPD

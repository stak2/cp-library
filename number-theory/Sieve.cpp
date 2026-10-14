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

struct Sieve{
    vector<bool> isprime;
    vector<int> minfactor;
    vector<int> mobius;
    Sieve(int n) : isprime(n+1,true), minfactor(n+1, -1), mobius(n+1,1){
        isprime[1] = false;
        minfactor[1] = 1;
 
        for(int i = 2; i <= n; i++){
            if(!isprime[i]) continue;
            minfactor[i] = i;
            mobius[i] = -1;
            for(int j = i+i; j <= n; j += i){
                isprime[j] = false;
                if(minfactor[j] == -1) minfactor[j] = i;
                if((j/i)%i == 0) mobius[j] = 0;
                else mobius[j] *= -1;
            }
        }
    }
 
    vector<pair<int, int>> factorize(int n){
        vector<pair<int, int>> res;
        while(n > 1){
            int p = minfactor[n];
            int e = 0;
            while(minfactor[n] == p){
                n /= p;
                e++;
            }
            res.push_back({p,e});
        }
        return res;
    }
 
    vector<int> divisor(int n){
        vector<int> res;
        res.push_back(1);
        auto v = factorize(n);
        for(auto x : v){
            int s = res.size();
            for(int i = 0; i < s; i++){
                int m = 1;
                for(int _ = 0; _ < x.second; _++){
                    m *= x.first;
                    res.push_back(res[i]*m);
                }
            }
        }
        return res;
    }
 
    template<class T> void fzt(vector<T> &f){
        int n = f.size();
        for(int p = 2; p < n; p++){
            if(!isprime[p]) continue;
            for(int q = (n-1)/p; q > 0; q--){
                f[q] += f[p*q];
            }
        }
    }
 
 
    template<class T> void fmt(vector<T> &f){
        int n = f.size();
        for(int p = 2; p < n; p++){
            if(!isprime[p]) continue;
            for(int q = 1; q*p < n; q++){
                f[q] -= f[p*q];
            }
        }

        // for(int p = 2; p < n; p++){
        //     if(!isprime[p]) continue;
        //     for(int q = (n-1)/p; q > 0; q--){
        //         f[p*q] -= f[q];
        //     }
        // }
    }
 
    template<class T> vector<T> conv(const vector<T> &f, const vector<T> &g){
        int n = max(f.size(), g.size());
        vector<T> nf(n), ng(n), h(n);
        for(int i = 0; i < (int)f.size(); i++) nf[i] = f[i];
        for(int i = 0; i < (int)g.size(); i++) ng[i] = g[i];
        fzt(nf); fzt(ng);
        for(int i = 0; i < n; i++) h[i] = nf[i]*ng[i];
        fmt(h);
        return h;
    }
};

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
using P = pair<int, int>;
using D = double;
using LD = long double;
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }

struct Factorization{
    vector<bool> isprime;
    vector<ll> primes;
    Factorization(ll n) : isprime(sqrtl(n)+1,true){
        isprime[1] = false;
        for(ll i = 2; i*i <= n; i++){
            if(!isprime[i]) continue;
            primes.push_back(i);
            for(ll j = i+i; j*j <=n; j += i){
                isprime[j] = false;
            }
        }
    }

    vector<pair<ll,int>> factorize(ll n){
        vector<pair<ll,int>> res;
        for(ll p : primes){
            /* if(p * p > n) break; */
            int e = 0;
            while(n%p == 0){
                n /= p;
                e++;
            }
            if(e > 0) res.emplace_back(p, e);
        }
        if(n > 1) res.emplace_back(n, 1);
        return res;
    }

    vector<ll> divisors(ll n){
        vector<ll> res{1};
        const auto& pf = factorize(n);
        for(auto x : pf){
            int s = res.size();
            for(int i = 0; i < s; i++){
                ll m = 1;
                for(int _ = 0; _ < x.second; _++){
                    m *= x.first;
                    res.push_back(res[i]*m);
                }
            }
        }
        return res;
    }
};

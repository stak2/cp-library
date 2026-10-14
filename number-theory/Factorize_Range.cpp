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
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }

struct rangesieve{
    ll l,r,m; // [l,r]
    vector<bool> isprime;
    vector<vector<ll>> prime_factors;
    rangesieve(ll l, ll r): l(l),r(r),m(sqrtl(r)+1),isprime(m,true),prime_factors(r-l+1){
        isprime[1] = false;
        for(ll p = 2; p <= m; p++){
            if(!isprime[p]) continue;
            for(ll q = p+p; q <= m; q += p){
                isprime[q] = false;
            }
            for(ll v = (l+p-1)/p*p; v <= r; v += p){
                prime_factors[v-l].pb(p);
            }
        }
    }

    vector<pair<ll,int>> factorize(ll n){
        vector<pair<ll,int>> res;
        const auto& pfs = prime_factors[n-l];
        for(ll p : pfs){
            int e = 0;
            while(n % p == 0){
                n /= p;
                e++;
            }
            if(e > 0) res.pb({p,e});
        }
        if(n > 1) res.pb({n,1}); 
        return res;
    }
};

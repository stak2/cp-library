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

struct nsieve{ // [1,n]
    vector<int> minfactor;
    vector<bool> isprime;
    
    nsieve(int n): isprime(n+1,true), minfactor(n+1,-1){
        isprime[1] = false;
        minfactor[1] = 1;

        for(int i=2; i<=n; i++){
            if(!isprime[i]) continue;
            minfactor[i] = i;
            for(int j=i+i; j<=n; j+=i){
                isprime[j] = false;
                if(minfactor[j] == -1) minfactor[j] = i;
            }
        }
    }

    vector<P> factorize(int n){
        vector<P> res;
        while(n > 1){
            int p = minfactor[n];
            int e = 0;
            while(minfactor[n] == p){
                e++;
                n /= p;
            }
            res.emplace_back(p, e);
        }
        return res;
    }
};

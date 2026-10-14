using ll = long long;

// floor(n/d) == q, d in (l,r]
template<typename F>
void enumerate_quotients(ll n, F f){
    ll r = n;
    while(r) {
        ll q = n/r;
        ll l = n/(q+1);
        f(l,r,q);
        r = l;
    }
}

// floor(a/d) == qa, floor(b/d) == qb, d in (l,r]
template<typename F>
void enumerate_quotients_pair(ll a, ll b, F f){
    ll r = max(a,b);
    while(r) {
        ll qa = a/r, qb = b/r;
        ll l = max(a/(qa+1), b/(qb+1));
        f(l,r,qa,qb);
        r = l;
    }
}

// ceil(n/d) == q, d in [l,r)
// r >= n: q = 1 excluded
template <typename F>
void enumerate_ceil(ll n, F f) {
    ll r = n;
    while(r > 1){
        ll q = (n+r-2)/(r-1);
        ll l = (n+q-1)/q;
        f(l,r,q);
        r = l;
    }
}

// ceil(a/d) == qa, ceil(b/d) == qb, d in [l,r)
// r >= max(a,b): qa = qb = 1 excluded
template <typename F>
void enumerate_ceil_pair(ll a, ll b, F f){
    ll r = max(a,b);
    while(r > 1) {
        ll qa = (a+r-2)/(r-1), qb = (b+r-2)/(r-1);
        ll l = max((a+qa-1)/qa, (b+qb-1)/qb);
        f(l,r,qa,qb);
        r = l;
    }
}

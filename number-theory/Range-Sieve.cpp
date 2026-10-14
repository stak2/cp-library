struct RangeSieve {
    ll l, r, m;
    // [l, r]
    vector<bool> isprime;
    vector<vector<pair<ll,int>>> factor;
    // vector<int> factor;
    vector<ll> a;
    RangeSieve(ll _l, ll _r): l(_l), r(_r), m((ll)(sqrtl(r))),
        isprime(m+1, true), factor(r-l+1), a(r-l+1) {

        iota(ALL(a), l);
        isprime[1] = false;
        for (ll p = 2; p <= m; p++) if (isprime[p]) {
            for (ll q = p*2; q <= m; q += p) {
                isprime[q] = false;
            }
            for (ll q = (l+p-1)/p * p; q <= r; q += p) {
                int e = 0;
                while (a[q-l] % p == 0) {
                    a[q-l] /= p;
                    e++;
                }
                factor[q-l].emplace_back(p, e);
                // factor[q-l]++;
            }
        }
        for (ll i = 0; i <= r-l; i++) if (a[i] != 1) {
            factor[i].emplace_back(a[i], 1);
            // factor[i]++;
        }
    }

    vector<pair<ll,int>> factorize(ll x) {
        assert(l <= x and x <= r);
        return factor[x-l];
    }

    // bool is_prime_power(ll x) {
    //     assert(l <= x and x <= r);
    //     return factor[x-l] == 1;
    // }
};
// [10^12, 10^12+10^6] で 0.6s, ~140 MB

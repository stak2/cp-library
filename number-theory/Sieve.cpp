struct Sieve{
    vector<bool> isprime;
    vector<int> minfactor;
    vector<int> mobius;
    Sieve(int n) : isprime(n+1,true), minfactor(n+1, -1), mobius(n+1,1) {
        isprime[1] = false;
        minfactor[1] = 1;
 
        for(int i = 2; i <= n; i++) {
            if (!isprime[i]) continue;
            minfactor[i] = i;
            mobius[i] = -1;
            for (int j = i+i; j <= n; j += i) {
                isprime[j] = false;
                if (minfactor[j] == -1) minfactor[j] = i;
                if ((j/i)%i == 0) mobius[j] = 0;
                else mobius[j] *= -1;
            }
        }
    }
 
    vector<pair<int, int>> factorize(int n){
        vector<pair<int, int>> res;
        while (n > 1){
            int p = minfactor[n];
            int e = 0;
            while (minfactor[n] == p) {
                n /= p;
                e++;
            }
            res.emplace_back(p, e);
        }
        return res;
    }
 
    vector<int> divisor(int n){
        vector<int> res{1};
        auto v = factorize(n);
        for (auto x : v) {
            int s = res.size();
            for (int i = 0; i < s; i++) {
                int m = 1;
                for (int _ = 0; _ < x.second; _++) {
                    m *= x.first;
                    res.emplace_back(res[i]*m);
                }
            }
        }
        return res;
    }
 
    // g[n] = \sum_{n | m} f[m]
    template<class T> void multiple_zeta(vector<T> &f){
        int n = f.size();
        for (int p = 2; p < n; p++) {
            if (!isprime[p]) continue;
            for (int k = (n-1)/p; k > 0; k--) {
                f[k] += f[p*k];
            }
        }
    }

    // g[n] = \sum_{d | n} f[d]
    template<class T> void divisor_zeta(vector<T> &f){
        int n = f.size();
        for (int p = 2; p < n; p++) {
            if (!isprime[p]) continue;
            for (int k = 1; k <= (n-1)/p; k++) {
                f[p*k] += f[k];
            }
        }
    }
 
    // f[n] = \sum_{n | m} g[m]
    // g[n] = \sum_{n | m} mu[m/n] f[m]
    template<class T> void multiple_mobius(vector<T> &f){
        int n = f.size();
        for (int p = 2; p < n; p++) {
            if (!isprime[p]) continue;
            for (int k = 1; k <= (n-1)/p; k++) {
                f[k] -= f[p*k];
            }
        }
    }

    // f[n] = \sum_{d | n} g[d]
    // g[n] = \sum_{d | n} mu[n/d] f[d]
    template<class T> void divisor_mobius(vector<T> &f){
        int n = f.size();
        for (int p = 2; p < n; p++) {
            if (!isprime[p]) continue;
            for (int k = (n-1)/p; k > 0; k--) {
                f[p*k] -= f[k];
            }
        }
    }
 
    // h[n] = \sum_{gcd(i,j)=n} f[i] g[j]
    template<class T> vector<T> gcd_convolution(vector<T> f, vector<T> g){
        int n = max(f.size(), g.size());
        multiple_zeta(f); multiple_zeta(g);
        for (int i = 0; i < n; i++) f[i] *= g[i];
        multiple_mobius(f);
        return f;
    }
};

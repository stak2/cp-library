// a_n = \sum_{i=0}^{k-1} a_{n-k+i} * d_i
// sum -> add
// * -> mul
// e: identity of add
template<typename T, T (*add)(T, T), T(*mul)(T, T), T (*e)()>
class Kitamasa {
private:
    vector<T> a;
    vector<T> d;
    int k;

    // find f(n) \in T^k s.t. a_n = \sum_{i=0}^{k-1} f(n)_i a_i
    vector<T> calc_f(int64_t n){
        if (n == k) return d;

        vector<T> f(k);
        if (n & 1 or n < k*2) {
            vector<T> pf = calc_f(n - 1);
            f[0] = mul(pf[k-1], d[0]);
            for (int i = 1; i < k; i++) f[i] = add(pf[i-1], mul(pf[k-1], d[i]));
        }
        else {
            vector<T> pf = calc_f(n >> 1);
            vector<T> cf = pf;
            for (int i = 0; i < k; i++) f[i] = mul(pf[0], cf[i]);
            for (int i = 1; i < k; i++) {
                T bk = cf[k-1];
                for (int j = k-1; j > 0; j--) cf[j] = add(cf[j-1], mul(bk, d[j]));
                cf[0] = mul(bk, d[0]);
                for (int j = 0; j < k; j++) f[j] = add(f[j], mul(pf[i], cf[j]));
            }
        }
        return f;
    }

public:
    Kitamasa(vector<T>& _a, vector<T>& _d) : a(_a), d(_d), k((int)a.size()) {}

    T get(int64_t n){
        if (n < k) return a[n];
        vector<T> f = calc_f(n);
        T res = e();
        for (int i = 0; i < k; ++i) res = add(res, mul(f[i], a[i]));
        return res;
    }
};

const ll INF = 1e18;
ll add(ll a, ll b) { return max(a,b); }
ll mul(ll a, ll b) { return a + b; }
ll e() { return -INF; }


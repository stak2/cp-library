#include <vector>
using namespace std;
using ll = long long;

/*
* dual segtree
* range apply / point get / point set
* operator (com) must be associative:
*   (f g) (x) = f(g(x))
* ex) range add, chmax/chmin, apply
* https://hackmd.io/@tatyam-prime/DualSegmentTree
*/
template< class T, T (*id)(), void(*com)(T&, const T&) >
struct DualSegmentTree {
    int sz;
    vector< T > dat;
    DualSegmentTree(int n){
        sz = 1;
        while(sz < n) sz <<= 1;
        dat.assign(2*sz, id());
    }
    DualSegmentTree(const vector< T > &v){
        sz = 1;
        while(sz < v.size()) sz <<= 1;
        dat.assign(2*sz, id());
        for(int i=0; i<v.size(); i++) dat[sz+i] = v[i];
    }

    void push(int p){
        if(!p) return;
        ll r = 31 - __builtin_clz(p);
        for(ll i = r; i > 0; i--){
            ll q = p >> i;
            if(dat[q] != id()){
                com(dat[q*2], dat[q]);
                com(dat[q*2+1], dat[q]);
                dat[q] = id();
            }
        }
    }

    T get(int p){
        p += sz;
        push(p);
        return dat[p];
    }

    void set(int p, const T &t){
        p += sz;
        push(p);
        dat[p] = t;
    }

    void apply(int l, int r, const T &t){
        if(l >= r) return;
        l += sz; r += sz;
        push(l >> __builtin_ctz(l));
        push((r >> __builtin_ctz(r))-1);
        for(;l<r;l>>=1,r>>=1){
            if(l&1) com(dat[l++], t);
            if(r&1) com(dat[--r], t);
        }
    }
};

using T = ll;
T id() {return 0;}
void com(T &a, const T &b) {a += b;}

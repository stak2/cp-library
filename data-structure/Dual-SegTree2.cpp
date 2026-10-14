#include <vector>
using namespace std;
using ll = long long;

/*
* dual segtree
* range apply / point get
* operator (com) must be commutative:
*   f g = g f
* ex) range add, chmax/chmin, (apply)
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

    void apply(int l, int r, const T &t){
        l += sz; r += sz;
        for(;l<r;l>>=1,r>>=1){
            if(l&1) com(dat[l++], t);
            if(r&1) com(dat[--r], t);
        }
    }

    T get(int i) const{
        T res = id();
        i += sz;
        for(;i;i>>=1){
            com(res, dat[i]);
        }
        return res;
    }
};

using T = ll;
T id() {return 0;}
void com(T &a, const T &b) {a += b;}

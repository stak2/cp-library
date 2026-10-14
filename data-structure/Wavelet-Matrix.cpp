#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <vector>

namespace wm {
using u64 = std::uint64_t;
using i64 = std::int64_t;
using i128 = __int128_t; // GCC / Clang, C++17

// 64-bit packed rank vector. rank1(i) counts ones in [0,i).
class BitVector {
    std::vector<u64> bits_;
    std::vector<int> prefix_;
public:
    explicit BitVector(int n = 0) : bits_((n + 63LL) / 64), prefix_(bits_.size() + 1) {}
    void set(int i) { bits_[i / 64] |= u64(1) << (i % 64); }
    void build() {
        for (std::size_t i = 0; i < bits_.size(); ++i)
            prefix_[i + 1] = prefix_[i] + __builtin_popcountll(bits_[i]);
    }
    int rank1(int i) const {
        int q = i / 64, b = i % 64;
        return prefix_[q] + (b ? __builtin_popcountll(bits_[q] & ((u64(1) << b) - 1)) : 0);
    }
};

// Static nonnegative keys in [0,2^H); the endpoint 2^H must fit Key.
// All index/value intervals are half-open; k is zero-based.
template<class Key = u64> class WaveletMatrix {
    static_assert(std::numeric_limits<Key>::is_integer && !std::is_same<Key,bool>::value, "Key must be an integer other than bool");
    int n_ = 0, h_ = 0;
    std::vector<BitVector> bits_;
    std::vector<int> zero_;
    void range(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        (void)l; (void)r;
    }
public:
    explicit WaveletMatrix(const std::vector<Key>& a = {}, int height = -1) {
        if (a.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("too many keys");
        n_ = static_cast<int>(a.size());
        if (std::any_of(a.begin(),a.end(),[](Key x) { return x < Key(0); }))
            throw std::invalid_argument("keys must be nonnegative");
        Key mx = a.empty() ? 0 : *std::max_element(a.begin(), a.end());
        h_ = height;
        if (h_ == -1) { h_ = 0; for (Key x = mx; x; x >>= 1) ++h_; }
        if (h_ < 0 || h_ >= std::numeric_limits<Key>::digits || mx >= (Key(1) << h_))
            throw std::invalid_argument("keys must belong to [0,2^H), and 2^H must fit Key");
        auto cur = a;
        std::vector<Key> next(n_);
        for (int d = 0; d < h_; ++d) {
            int b = h_ - 1 - d;
            bits_.emplace_back(n_);
            for (int i = 0; i < n_; ++i) if ((cur[i] >> b) & 1) bits_.back().set(i);
            bits_.back().build();
            int z = n_ - bits_.back().rank1(n_);
            zero_.push_back(z);
            int p = 0, q = z;
            for (Key x : cur) next[((x >> b) & 1) ? q++ : p++] = x;
            cur.swap(next);
        }
    }
    int size() const { return n_; }
    int height() const { return h_; }
    Key domain_end() const { return Key(1) << h_; }
    // Maps a level-d interval to its zero/one child at level d+1.
    std::pair<int,int> child(int d, int l, int r, bool one) const {
        assert(0 <= d && d < h_);
        int a = bits_[d].rank1(l), b = bits_[d].rank1(r);
        return one ? std::make_pair(zero_[d] + a, zero_[d] + b)
                   : std::make_pair(l - a, r - b);
    }
    int next_index(int d, int i, bool one) const {
        int a = bits_[d].rank1(i);
        return one ? zero_[d] + a : i - a;
    }
    // Visits O(H) disjoint nodes covering keys in [a,b).
    // f(depth,left,right) sees the corresponding permuted weight interval.
    template<class F> void visit(int l, int r, Key a, Key b, F&& f) const {
        range(l,r);
        a = std::max(a, Key(0));
        b = std::min(b, domain_end());
        if (a >= b) return;
        auto rec = [&](auto&& self, int d, int x, int y, Key lo, Key hi) -> void {
            if (x == y || hi <= a || b <= lo) return;
            if (a <= lo && hi <= b) { f(d,x,y); return; }
            Key mid = lo + (hi-lo)/2;
            auto z = child(d,x,y,false), o = child(d,x,y,true);
            self(self,d+1,z.first,z.second,lo,mid);
            self(self,d+1,o.first,o.second,mid,hi);
        };
        rec(rec,0,l,r,0,domain_end());
    }
    Key kth_smallest_xor(int l, int r, int k, Key x) const {
        range(l,r); assert(0 <= k && k < r-l);
        if (x < Key(0) || x >= domain_end()) throw std::invalid_argument("xor mask outside domain");
        Key answer = 0;
        for (int d = 0; d < h_; ++d) {
            int b = h_-1-d;
            bool flip = (x >> b) & 1;
            auto z = child(d,l,r,flip);
            if (k < z.second-z.first) { l=z.first; r=z.second; }
            else {
                k -= z.second-z.first;
                answer |= Key(1) << b;
                auto o = child(d,l,r,!flip); l=o.first; r=o.second;
            }
        }
        return answer; // The XORed value, not the original key.
    }
    Key kth_smallest(int l, int r, int k) const { return kth_smallest_xor(l,r,k,0); }
    int count_less(int l, int r, Key x) const {
        range(l,r);
        if (x <= Key(0)) return 0;
        if (x >= domain_end()) return r-l;
        int answer = 0;
        for (int d = 0; d < h_; ++d) {
            auto z = child(d,l,r,false);
            if ((x >> (h_-1-d)) & 1) {
                answer += z.second-z.first;
                auto o = child(d,l,r,true); l=o.first; r=o.second;
            } else { l=z.first; r=z.second; }
        }
        return answer;
    }
    int count(int l, int r, Key a, Key b) const {
        range(l,r); return a >= b ? 0 : count_less(l,r,b)-count_less(l,r,a);
    }
    // callback receives weights in the permutation at each depth (including 0,H).
    template<class T, class F>
    void weight_levels(std::vector<T> w, const std::vector<Key>& keys, F&& f) const {
        if (w.size() != static_cast<std::size_t>(n_) || keys.size() != w.size())
            throw std::invalid_argument("weight/key size mismatch");
        auto cur = keys;
        std::vector<T> nw(n_);
        std::vector<Key> nk(n_);
        f(0,w);
        for (int d=0; d<h_; ++d) {
            int p=0, q=zero_[d], b=h_-1-d;
            for (int i=0; i<n_; ++i) {
                int j=((cur[i]>>b)&1) ? q++ : p++;
                nw[j]=w[i]; nk[j]=cur[i];
            }
            w.swap(nw); cur.swap(nk); f(d+1,w);
        }
    }
};

// Weight is the input type; prefix sums and search arithmetic use Sum.
template<class Key = u64, class Weight = Key, class Sum = long long>
class WaveletMatrixSum {
    WaveletMatrix<Key> matrix_;
    std::vector<std::vector<Sum>> prefix_;
    bool key_weights_ = false;
    Sum node_sum(int d, int l, int r) const { return prefix_[d][r]-prefix_[d][l]; }
    void build(const std::vector<Key>& a, const std::vector<Weight>& w) {
        matrix_.weight_levels(w,a,[&](int, const auto& v) {
            prefix_.emplace_back(v.size()+1,0);
            for (std::size_t i=0;i<v.size();++i) prefix_.back()[i+1]=prefix_.back()[i]+static_cast<Sum>(v[i]);
        });
    }
public:
    explicit WaveletMatrixSum(const std::vector<Key>& a, int h=-1)
        : matrix_(a,h), key_weights_(true) {
        // Preserve key values even when Weight is narrower than Key.
        matrix_.weight_levels(a,a,[&](int,const auto& v) {
            prefix_.emplace_back(v.size()+1,Sum(0));
            for (std::size_t i=0;i<v.size();++i) prefix_.back()[i+1]=prefix_.back()[i]+static_cast<Sum>(v[i]);
        });
    }
    WaveletMatrixSum(const std::vector<Key>& a, const std::vector<Weight>& w, int h=-1)
        : matrix_(a,h) { build(a,w); }
    const WaveletMatrix<Key>& matrix() const { return matrix_; }
    Sum sum(int l,int r,Key a,Key b) const {
        Sum ans=0;
        matrix_.visit(l,r,a,b,[&](int d,int x,int y) { ans+=node_sum(d,x,y); });
        return ans;
    }
    Sum sum_less(int l,int r,Key x) const { return sum(l,r,0,x); }
    // Advanced 6: maximum x <= 2^H with sum(A_i < x) <= budget.
    Key max_threshold(int l,int r,Sum budget) const {
        if (!key_weights_ || budget < 0)
            throw std::invalid_argument("search requires key weights and nonnegative budget");
        assert(0 <= l && l <= r && r <= matrix_.size());
        if (node_sum(0,l,r) <= budget) return matrix_.domain_end();
        Key value=0;
        for (int d=0;d<matrix_.height();++d) {
            auto z=matrix_.child(d,l,r,false);
            Sum s=node_sum(d+1,z.first,z.second);
            if (s <= budget) {
                budget-=s; value |= Key(1) << (matrix_.height()-1-d);
                auto o=matrix_.child(d,l,r,true); l=o.first; r=o.second;
            } else { l=z.first; r=z.second; }
        }
        return value; // first key whose entire multiplicity would exceed budget
    }
    // Advanced 7: maximum x with sum(max(x-A_i,0)) <= budget, O(H).
    // Nonempty interval is necessary: an empty interval has no finite maximum.
    Sum max_raise(int l,int r,Sum budget) const {
        static_assert(std::numeric_limits<Sum>::is_integer, "max_raise requires an integer Sum type");
        if (!key_weights_ || budget < 0)
            throw std::invalid_argument("search requires key weights and nonnegative budget");
        assert(0 <= l && l < r && r <= matrix_.size());
        int total=r-l;
        Sum all=node_sum(0,l,r), end=matrix_.domain_end();
        if (end*total-all <= budget) return (budget+all)/total;
        Sum used_sum=0;
        int used_count=0;
        Key lo=0,hi=matrix_.domain_end();
        for (int d=0;d<matrix_.height();++d) {
            auto z=matrix_.child(d,l,r,false);
            int c=used_count+z.second-z.first;
            Sum s=used_sum+node_sum(d+1,z.first,z.second);
            Key mid=lo+(hi-lo)/2;
            if (Sum(mid)*c-s <= budget) {
                used_count=c; used_sum=s; lo=mid;
                auto o=matrix_.child(d,l,r,true); l=o.first; r=o.second;
            } else { hi=mid; l=z.first; r=z.second; }
        }
        return lo;
    }
};

// Segtree monoid must be commutative; unit is an identity element.
template<class T, class Op> class SegTree {
    int base_=1;
    T unit_;
    Op op_;
    std::vector<T> tree_;
public:
    SegTree(const std::vector<T>& a,T unit,Op op) : unit_(unit),op_(op) {
        while (base_ < static_cast<int>(a.size())) base_*=2;
        tree_.assign(2*static_cast<std::size_t>(base_),unit_);
        std::copy(a.begin(),a.end(),tree_.begin()+base_);
        for (int i=base_-1;i;--i) tree_[i]=op_(tree_[2*i],tree_[2*i+1]);
    }
    void set(int i,T value) {
        i+=base_; tree_[i]=value;
        while (i>>=1) tree_[i]=op_(tree_[2*i],tree_[2*i+1]);
    }
    T fold(int l,int r) const {
        T x=unit_,y=unit_;
        for (l+=base_,r+=base_;l<r;l>>=1,r>>=1) {
            if (l&1) x=op_(x,tree_[l++]);
            if (r&1) y=op_(tree_[--r],y);
        }
        return op_(x,y);
    }
};

// Keys are static; weights can be replaced at original indices.
template<class Key,class T,class Op> class WaveletMatrixMonoid {
    WaveletMatrix<Key> matrix_;
    std::vector<Key> keys_;
    T unit_;
    Op op_;
    std::vector<SegTree<T,Op>> trees_;
public:
    WaveletMatrixMonoid(const std::vector<Key>& a,const std::vector<T>& w,T unit,Op op,int h=-1)
        : matrix_(a,h),keys_(a),unit_(unit),op_(op) {
        matrix_.weight_levels(w,a,[&](int,const auto& v) { trees_.emplace_back(v,unit_,op_); });
    }
    T fold(int l,int r,Key a,Key b) const {
        T ans=unit_;
        matrix_.visit(l,r,a,b,[&](int d,int x,int y) { ans=op_(ans,trees_[d].fold(x,y)); });
        return ans;
    }
    void set(int i,T value) {
        assert(0 <= i && i < matrix_.size());
        Key key=keys_[i];
        trees_[0].set(i,value);
        for (int d=0;d<matrix_.height();++d) {
            i=matrix_.next_index(d,i,(key>>(matrix_.height()-1-d))&1);
            trees_[d+1].set(i,value);
        }
    }
    const WaveletMatrix<Key>& matrix() const { return matrix_; }
};

// nullopt is the max identity: negative weights, including INT64_MIN, are valid.
template<class Weight = i64> struct OptionalMax {
    std::optional<Weight> operator()(std::optional<Weight> a,std::optional<Weight> b) const {
        if (!a) return b;
        if (!b) return a;
        return std::max(*a,*b);
    }
};
template<class Key = u64, class Weight = i64>
using WaveletMatrixMax = WaveletMatrixMonoid<Key,std::optional<Weight>,OptionalMax<Weight>>;

// Coordinate compression preserves order, but NOT XOR or numeric sums of keys.
template<class Coordinate = i64> class PointIndex {
    std::vector<Coordinate> xs_,ys_;
    std::vector<u64> keys_;
    std::vector<int> original_to_sorted_;
public:
    explicit PointIndex(const std::vector<std::pair<Coordinate,Coordinate>>& points) {
        if (points.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("too many points");
        int n=static_cast<int>(points.size());
        std::vector<int> order(n); std::iota(order.begin(),order.end(),0);
        std::stable_sort(order.begin(),order.end(),[&](int i,int j) { return points[i]<points[j]; });
        for (const auto& p:points) ys_.push_back(p.second);
        std::sort(ys_.begin(),ys_.end()); ys_.erase(std::unique(ys_.begin(),ys_.end()),ys_.end());
        original_to_sorted_.resize(n);
        for (int j=0;j<n;++j) {
            int i=order[j]; original_to_sorted_[i]=j; xs_.push_back(points[i].first);
            keys_.push_back(std::lower_bound(ys_.begin(),ys_.end(),points[i].second)-ys_.begin());
        }
    }
    const std::vector<u64>& keys() const { return keys_; }
    int position(int id) const { assert(0 <= id && id < static_cast<int>(xs_.size())); return original_to_sorted_[id]; }
    struct Box { int l,r; u64 a,b; };
    Box box(Coordinate l,Coordinate r,Coordinate d,Coordinate u) const {
        if (l>=r || d>=u) return {0,0,0,0};
        return {static_cast<int>(std::lower_bound(xs_.begin(),xs_.end(),l)-xs_.begin()),
                static_cast<int>(std::lower_bound(xs_.begin(),xs_.end(),r)-xs_.begin()),
                static_cast<u64>(std::lower_bound(ys_.begin(),ys_.end(),d)-ys_.begin()),
                static_cast<u64>(std::lower_bound(ys_.begin(),ys_.end(),u)-ys_.begin())};
    }
};

template<class Coordinate = i64> class RectangleCount {
    PointIndex<Coordinate> index_;
    WaveletMatrix<u64> matrix_;
public:
    explicit RectangleCount(const std::vector<std::pair<Coordinate,Coordinate>>& p) : index_(p),matrix_(index_.keys()) {}
    int count(Coordinate l,Coordinate r,Coordinate d,Coordinate u) const {
        auto b=index_.box(l,r,d,u); return matrix_.count(b.l,b.r,b.a,b.b);
    }
};

// Advanced 9: declare every possible coordinate first (offline preprocessing).
// Each declared slot has its own multiset, so duplicate triples are supported.
template<class Coordinate = i64, class Weight = i64> class OfflineRectangleMax {
    PointIndex<Coordinate> index_;
    WaveletMatrixMax<u64,Weight> matrix_;
    std::vector<std::multiset<Weight>> weights_;
    void refresh(int id) {
        const auto& s=weights_[id];
        matrix_.set(index_.position(id),s.empty() ? std::nullopt : std::optional<Weight>(*s.rbegin()));
    }
public:
    explicit OfflineRectangleMax(const std::vector<std::pair<Coordinate,Coordinate>>& coordinates)
        : index_(coordinates),matrix_(index_.keys(),std::vector<std::optional<Weight>>(coordinates.size()),std::nullopt,OptionalMax<Weight>{}),
          weights_(coordinates.size()) {}
    void insert(int id,Weight x) {
        assert(0 <= id && id < static_cast<int>(weights_.size()));
        weights_[id].insert(x); refresh(id);
    }
    // Erases ONE copy. Returns false if that triple is absent.
    bool erase(int id,Weight x) {
        assert(0 <= id && id < static_cast<int>(weights_.size()));
        auto it=weights_[id].find(x);
        if (it==weights_[id].end()) return false;
        weights_[id].erase(it); refresh(id); return true;
    }
    std::optional<Weight> max(Coordinate l,Coordinate r,Coordinate d,Coordinate u) const {
        auto b=index_.box(l,r,d,u); return matrix_.fold(b.l,b.r,b.a,b.b);
    }
};
} // namespace wm
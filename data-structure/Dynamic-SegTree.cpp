template <class S, S (*op)(S, S), S (*e)()>
class DynamicSegmentTree {
private:
    struct Node {
        size_t idx;
        S val, prod;
        int left, right;

        Node(size_t _idx, S _val): idx(_idx), val(_val), prod(_val),
            left(-1), right(-1) {}
    };

    size_t n;
    vector<Node> nodes;
    int root;

    S get_prod(int nid) const {
        return nid == -1 ? e() : nodes[nid].prod;
    }

    void set(int &nid, size_t low, size_t high, size_t pos, S x) {
        if (nid == -1) {
            nid = (int)nodes.size();
            nodes.emplace_back(Node(pos, x));
            return;
        }
        if (nodes[nid].idx == pos) {
            nodes[nid].val = x;
            nodes[nid].prod = op(get_prod(nodes[nid].left), op(x, get_prod(nodes[nid].right)));
            return;
        }
        size_t mid = (low + high) >> 1;
        if (pos < mid) {
            if (pos > nodes[nid].idx) swap(nodes[nid].idx, pos), swap(nodes[nid].val, x);
            set(nodes[nid].left, low, mid, pos, x);
        }
        else {
            if (pos < nodes[nid].idx) swap(pos, nodes[nid].idx), swap(x, nodes[nid].val);
            set(nodes[nid].right, mid, high, pos, x);
        }
        nodes[nid].prod = op(get_prod(nodes[nid].left), op(nodes[nid].val, get_prod(nodes[nid].right)));
    }

    S get(const int nid, size_t low, size_t high, size_t pos) const {
        if (nid == -1) return e();
        if (nodes[nid].idx == pos) return nodes[nid].val;
        size_t mid = (low + high) >> 1;
        if (pos < mid) return get(nodes[nid].left, low, mid, pos);
        else return get(nodes[nid].right, mid, high, pos);
    }

    S prod(const int nid, size_t low, size_t high, size_t l, size_t r) const {
        if (nid == -1 or high <= l or r <= low) return e();
        if (l <= low and high <= r) return nodes[nid].prod;
        size_t mid = (low + high) >> 1;
        S res = prod(nodes[nid].left, low, mid, l, r);
        if (l <= nodes[nid].idx and nodes[nid].idx < r) res = op(res, nodes[nid].val);
        return op(res, prod(nodes[nid].right, mid, high, l, r));
    }

    template <typename F>
    size_t max_right(const int nid, size_t low, size_t high, size_t l, const F& f, S& s) const {
        if (nid == -1 or high <= l) return n;
        if (f(op(s, nodes[nid].prod))) {
            s = op(s, nodes[nid].prod);
            return n;
        }
        size_t mid = (low + high) >> 1;
        size_t res = max_right(nodes[nid].left, low, mid, l, f, s);
        if (res != n) return res;
        if (l <= nodes[nid].idx) {
            s = op(s, nodes[nid].val);
            if (!f(s)) return nodes[nid].idx;
        }
        return max_right(nodes[nid].right, mid, high, l, f, s);
    }

    template <typename F>
    size_t min_left(const int nid, size_t low, size_t high, size_t r, const F& f, S& s) const {
        if (nid == -1 or r <= low) return 0;
        if (f(op(nodes[nid].prod, s))) {
            s = op(nodes[nid].prod, s);
            return 0;
        }
        size_t mid = (low + high) >> 1;
        size_t res = min_left(nodes[nid].right, mid, high, r, f, s);
        if (res != 0) return res;
        if (nodes[nid].idx < r) {
            s = op(nodes[nid].val, s);
            if (!f(s)) return nodes[nid].idx;
        }
        return min_left(nodes[nid].left, low, mid, r, f, s);
    }

public:
    explicit DynamicSegmentTree(size_t _n, int _q = -1): n(_n), root(-1) {
        if (_q > 0) nodes.reserve(_q);
    }

    void set(size_t p, S x) {
        assert(p < n);
        set(root, 0, n, p, x);
    }

    S get(size_t p) const {
        assert(p < n);
        return get(root, 0, n, p);
    }

    S prod(size_t l, size_t r) const {
        assert(l <= r and r <= n);
        return prod(root, 0, n, l, r);
    }

    S all_prod() const { return get_prod(root); }

    template <bool (*f)(S)>
    size_t max_right(size_t l) const {
        return max_right(l, [](S x) { return f(x); });
    }

    template <class F>
    size_t max_right(size_t l, const F &f) const {
        assert(l <= n);
        S s = e();
        assert(f(s));
        return max_right(root, 0, n, l, f, s);
    }

    template <bool (*f)(S)>
    size_t min_left(size_t r) const {
        return min_left(r, [](S x) { return f(x); });
    }

    template <class F>
    size_t min_left(size_t r, const F &f) const {
        assert(r <= n);
        S s = e();
        assert(f(s));
        return min_left(root, 0, n, r, f, s);
    }
};



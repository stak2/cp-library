template <class S, S (*op)(S, S), S (*e)()>
class DynamicSegmentTree {
private:
    struct Node {
        size_t idx;
        S val, prod;
        Node *left, *right;

        Node(size_t _idx, S _val): idx(_idx), val(_val), prod(_val),
            left(nullptr), right(nullptr) {}
    };

    size_t n;
    Node *root;

    S get_prod(const Node *ptr) const {
        return ptr ? ptr->prod : e();
    }

    Node *set(Node *ptr, size_t low, size_t high, size_t pos, S x) {
        if (!ptr) {
            return ptr = new Node(pos, x);
        }
        if (ptr->idx == pos) {
            ptr->val = x;
            ptr->prod = op(get_prod(ptr->left), op(x, get_prod(ptr->right)));
            return ptr;
        }
        size_t mid = (low + high) >> 1;
        if (pos < mid) {
            if (pos > ptr->idx) swap(ptr->idx, pos), swap(ptr->val, x);
            ptr->left = set(ptr->left, low, mid, pos, x);
        }
        else {
            if (pos < ptr->idx) swap(pos, ptr->idx), swap(x, ptr->val);
            ptr->right = set(ptr->right, mid, high, pos, x);
        }
        ptr->prod = op(get_prod(ptr->left), op(ptr->val, get_prod(ptr->right)));
        return ptr;
    }

    S get(const Node *ptr, size_t low, size_t high, size_t pos) const {
        if (!ptr) return e();
        if (ptr->idx == pos) return ptr->val;
        size_t mid = (low + high) >> 1;
        if (pos < mid) return get(ptr->left, low, mid, pos);
        else return get(ptr->right, mid, high, pos);
    }

    S prod(const Node *ptr, size_t low, size_t high, size_t l, size_t r) const {
        if (!ptr or high <= l or r <= low) return e();
        if (l <= low and high <= r) return ptr->prod;
        size_t mid = (low + high) >> 1;
        S res = prod(ptr->left, low, mid, l, r);
        if (l <= ptr->idx and ptr->idx < r) res = op(res, ptr->val);
        return op(res, prod(ptr->right, mid, high, l, r));
    }

    template <typename F>
    size_t max_right(Node *ptr, size_t low, size_t high, size_t l, const F& f, S& s) const {
        if (!ptr or high <= l) return n;
        if (f(op(s, ptr->prod))) {
            s = op(s, ptr->prod);
            return n;
        }
        size_t mid = (low + high) >> 1;
        size_t res = max_right(ptr->left, low, mid, l, f, s);
        if (res != n) return res;
        if (l <= ptr->idx) {
            s = op(s, ptr->val);
            if (!f(s)) return ptr->idx;
        }
        return max_right(ptr->right, mid, high, l, f, s);
    }

    template <typename F>
    size_t min_left(Node *ptr, size_t low, size_t high, size_t r, const F& f, S& s) const {
        if (!ptr or r <= low) return 0;
        if (f(op(ptr->prod, s))) {
            s = op(ptr->prod, s);
            return 0;
        }
        size_t mid = (low + high) >> 1;
        size_t res = min_left(ptr->right, mid, high, r, f, s);
        if (res != 0) return res;
        if (ptr->idx < r) {
            s = op(ptr->val, s);
            if (!f(s)) return ptr->idx;
        }
        return min_left(ptr->left, low, mid, r, f, s);
    }

public:
    explicit DynamicSegmentTree(size_t _n): n(_n), root(nullptr) {}

    void set(size_t p, S x) {
        assert(p < n);
        root = set(root, 0, n, p, x);
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


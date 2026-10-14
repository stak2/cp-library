template <class S, S (*e)(), class I = int>
struct IntervalSet {
    struct node {
        I l, r;
        S x;
        node (I l_, I r_, const S &x_): l(l_), r(r_), x(x_) {}
        constexpr bool operator<(const node &other) const {
            if (l != other.l) return l < other.l;
            return r < other.r;
        }
    };

    set<node> s;
    explicit IntervalSet() {}

    typename set<node>::iterator find(const I &x) {
        auto it = s.upper_bound(node(x, numeric_limits<I>::max(), e()));
        if (it == s.begin()) return s.end();
        it = prev(it);
        if (it->l <= x and x < it->r) return it;
        return s.end();
    }

    typename set<node>::iterator lower_bound(const I &x) {
        auto it = s.upper_bound(node(x, numeric_limits<I>::max(), e()));
        if (it == s.begin()) return s.end();
        return prev(it);
    }

    S get(const I &x) {
        auto it = find(x);
        if (it == s.end()) return e();
        return it->x;
    }

    template <typename ADD, typename DEL>
    void update(I l, I r, const S &x, const ADD &add, const DEL &del) {
        auto it = s.lower_bound(node{l, 0, x});
        while (it != s.end() and it->l <= r) {
            if (it->l == r) {
                if (it->x == x) {
                    del(r, it->r, x);
                    r = it->r, it = s.erase(it);
                }
                break;
            }
            if (it->r <= r) {
                del(it->l, it->r, it->x);
                it = s.erase(it);
            }
            else {
                if (it->x == x) {
                    r = it->r;
                    del(it->l, it->r, it->x);
                    it = s.erase(it);
                }
                else {
                    del(it->l, r, it->x);
                    node n = *it;
                    it = s.erase(it);
                    it = s.emplace_hint(it, r, n.r, n.x);
                }
            }
        }
        if (it != s.begin()) {
            it = prev(it);
            if (it->r == l) {
                if (it->x == x) {
                    del(it->l, it->r, it->x);
                    l = it->l;
                    it = s.erase(it);
                }
            }
            else if (l < it->r) {
                if (it->x == x) {
                    del(it->l, it->r, it->x);
                    l = min(l, it->l);
                    r = max(r, it->r);
                    it = s.erase(it);
                }
                else {
                    if (r < it->r) {
                        it = s.emplace_hint(next(it), r, it->r, it->x);
                        it = prev(it);
                    }
                    del(l, min(r, it->r), it->x);
                    node n = *it;
                    it = s.erase(it);
                    it = s.emplace_hint(it, n.l, l, n.x);
                }
            }
        }
        if (it != s.end()) it = next(it);
        add(l, r, x);
        s.emplace_hint(it, l, r, x);
    }

    void update(I l, I r, const S &x) {
        update(l, r, x, [](I, I, S){}, [](I, I, S){});
    }
};

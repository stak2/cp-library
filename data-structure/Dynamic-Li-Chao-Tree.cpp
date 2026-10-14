template <typename T, T x_low, T x_high, T y_id, bool minimize = true>
struct DynamicLiChaoTree {
    struct Line {
        T a, b;
        Line(T a_, T b_): a(a_), b(b_) {}
        inline T operator()(T x) const { return a * x + b; }
    };

    struct Node {
        Line f;
        int li, ri;
        Node(const Line &f_): f{f_}, li{-1}, ri{-1} {}
    };

    vector<Node> nodes;
    int root;
    DynamicLiChaoTree(): root(-1) {}

    void add_line(int &cur, Line &nf, const T &l, const T &r, const T &nfl, const T&nfr) {
        if (cur == -1) {
            cur = (int)nodes.size();
            nodes.push_back(Node(nf));
            return;
        }
        T cfl = nodes[cur].f(l), cfr = nodes[cur].f(r);
        if (cfl <= nfl and cfr <= nfr) {
            return;
        }
        else if (nfl <= cfl and nfr <= cfr) {
            nodes[cur].f = nf;
            return;
        }
        else {
            T m = (l + r) / 2;
            if (m == r) m--;
            T cfm = nodes[cur].f(m), nfm = nf(m);
            if (nfm < cfm) {
                swap(nodes[cur].f, nf);
                if (nfl >= cfl) {
                    add_line(nodes[cur].li, nf, l, m, cfl, cfm);
                }
                else{
                    add_line(nodes[cur].ri, nf, m + 1, r, cfm + nf.a, cfr);
                }
            }
            else {
                if (nfl <= cfl) {
                    add_line(nodes[cur].li, nf, l, m, nfl, nfm);
                }
                else{
                    add_line(nodes[cur].ri, nf, m + 1, r, nfm + nf.a, nfr);
                }
            }
        }
    }

    void add_line(T a, T b) {
        if constexpr (!minimize) a = -a, b = -b;
        Line f(a, b);
        add_line(root, f, x_low, x_high, f(x_low), f(x_high));
    }

    void add_segment(int &cur, Line &nf, const T &i, const T &j,
                     const T &l, const T &r, const T &nfl, const T &nfr) {
        if (r < i or j < l) return;
        if (i <= l and r <= j) {
            Line ff{nf};
            add_line(cur, ff, l, r, nfl, nfr);
            return;
        }
        if (cur == -1) {
            cur = (int)nodes.size();
            nodes.push_back(Node(Line(0, y_id)));
        }
        else {
            T cfl = nodes[cur].f(l), cfr = nodes[cur].f(r);
            if (cfl <= nfl && cfr <= nfr) return;
        }
        T m = (l + r) / 2;
        if (m == r) m--;
        T nfm = nf(m);
        add_segment(nodes[cur].li, nf, i, j, l, m, nfl, nfm);
        add_segment(nodes[cur].ri, nf, i, j, m + 1, r, nfm + nf.a, nfr);
    }

    void add_segment(const T &i, const T &j, T a, T b) {
        if constexpr (!minimize) a = -a, b = -b;
        Line f(a, b);
        add_segment(root, f, i, j - 1, x_low, x_high, f(x_low), f(x_high));
    }

    T query(const int &cur, const T &l, const T &r, const T &x) const {
        if (cur == -1) return y_id;
        if (l == r) return nodes[cur].f(x);
        T m = (l + r) / 2;
        if (m == r) m--;
        if (x <= m) return min(nodes[cur].f(x), query(nodes[cur].li, l, m, x));
        else return min(nodes[cur].f(x), query(nodes[cur].ri, m+1, r, x));
    }

    T query(const T &x) const {
        if constexpr (minimize) return query(root, x_low, x_high, x);
        else return -query(root, x_low, x_high, x);
    }
};


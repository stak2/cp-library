template<typename T, int MAX_LOG>
struct BinaryTrie {
    struct Node {
        int nxt[2];
        int exist;
        vector<int> accept;

        Node(): nxt{-1, -1}, exist(0) {}
    };

    vector<Node> nodes;
    int root;
    T lazy;

    explicit BinaryTrie(): root(0), lazy(T(0)) {
        nodes.emplace_back(Node());
    }

    void insert(const T &x, int delta, int idx = -1) {
        int n_cur = root;
        for (int depth = MAX_LOG - 1; depth >= 0; depth--) {
            bool nxt_01 = (x >> depth & 1) ^ (lazy >> depth & 1);
            if (nodes[n_cur].nxt[nxt_01] == -1) {
                nodes[n_cur].nxt[nxt_01] = (int)nodes.size();
                nodes.emplace_back(Node());
            }
            nodes[n_cur].exist += delta;
            n_cur = nodes[n_cur].nxt[nxt_01];
        }
        nodes[n_cur].exist += delta;
        if(idx != -1) nodes[n_cur].accept.emplace_back(idx);
    }

    void insert(const T &x, int idx = -1) {
        insert(x, 1, idx);
    }

    void erase(const T &x) {
        insert(x, -1, -1);
    }

    int find(const T &x) const {
        int n_cur = root;
        for (int depth = MAX_LOG - 1; depth >= 0; depth--) {
            bool nxt_01 = (x >> depth & 1) ^ (lazy >> depth & 1);
            int n_nxt = nodes[n_cur].nxt[nxt_01];
            if (n_nxt == -1) return -1;
            n_cur = n_nxt;
        }
        return n_cur;
    }

    int count(const T &x) const {
        int nx = find(x);
        return nx == -1 ? 0 : nodes[nx].exist;
    }

    pair<int, int> kth_element(int n_cur, int k, int depth) const {
        if (depth == -1) return make_pair(0, n_cur);
        bool nxt_01 = (lazy >> depth) & 1;
        int comp = nodes[n_cur].nxt[nxt_01] == -1 ?
            0 : nodes[nodes[n_cur].nxt[nxt_01]].exist;
        if (comp <= k) {
            pair<int, int> ret = kth_element(
                nodes[n_cur].nxt[nxt_01 ^ 1], k - comp, depth - 1);
            ret.first |= T(1) << depth;
            return ret;
        }
        else{
            return kth_element(nodes[n_cur].nxt[nxt_01], k, depth-1);
        }
    }

    pair<int, int> kth_element(int k) const {
        assert(0 <= k and k < nodes[root].exist);
        int n_cur = root;
        T ret = 0;
        for (int depth = MAX_LOG - 1; depth >= 0; depth--) {
            bool nxt_01 = (lazy >> depth) & 1;
            int comp = nodes[n_cur].nxt[nxt_01] == -1 ?
                0 : nodes[nodes[n_cur].nxt[nxt_01]].exist;
            if (comp <= k) {
                k -= comp;
                n_cur = nodes[n_cur].nxt[nxt_01 ^ 1];
                ret |= T(1) << depth;
            }
            else{
                n_cur = nodes[n_cur].nxt[nxt_01];
            }
        }
        // return make_pair(ret, n_cur);
        return kth_element(root, k, MAX_LOG - 1);
    }

    pair<int, int> min_element() const {
        assert(nodes[root].exist > 0);
        return kth_element(0);
    }

    pair<int, int> max_element() const {
        assert(nodes[root].exist > 0);
        return kth_element(nodes[root].exist - 1);
    }

    int count_less(int n_cur, const T &x, int depth) const {
        if (depth == -1) return 0;
        int ret = 0;
        bool nxt_01 = (lazy >> depth) & 1;
        bool xi = (x >> depth) & 1;
        if (xi and nodes[n_cur].nxt[nxt_01] != -1)
            ret += nodes[nodes[n_cur].nxt[nxt_01]].exist;
        if (nodes[n_cur].nxt[nxt_01 ^ xi] != -1)
            ret += count_less(nodes[n_cur].nxt[nxt_01 ^ xi], x, depth - 1);
        return ret;
    }

    int count_less(const T &x) const {
        /*
        int ret = 0;
        int n_cur = root;
        for (int depth = MAX_LOG - 1; depth >= 0; depth--) {
            bool nxt_01 = (lazy >> depth) & 1;
            bool xi = (x >> depth) & 1;
            if (xi and nodes[n_cur].nxt[nxt_01] != -1) {
                ret += nodes[nodes[n_cur].nxt[nxt_01]].exist;
            }
            if (nodes[n_cur].nxt[nxt_01 ^ xi] != -1) {
                n_cur = nodes[n_cur].nxt[nxt_01 ^ xi];
            }
            else break;
        }
        return ret;
        */
        return count_less(root, x, MAX_LOG - 1);
    }

    int count_leq(const T &x) const {
        return count_less(root, x+1, MAX_LOG - 1);
    }

    int count_greater(const T &x) const {
        return nodes[root].exist - count_less(x+1);
    }

    int count_geq(const T &x) const {
        return nodes[root].exist - count_less(x);
    }

    void operate_xor(const T &x) {
        lazy ^= x;
    }
};

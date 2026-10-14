template <typename T, int MAX_LOG>
struct BinaryTrie {
public:
    struct Node {
        Node *nxt[2];
        int exist;
        vector<int> accept;

        Node(): nxt{nullptr, nullptr}, exist(0) {}
    };

    Node *root;
    T lazy;

    explicit BinaryTrie(): root(new Node()), lazy(T(0)) {}

    void insert(const T &x, int idx = -1) {
        insert(root, x, MAX_LOG, 1, idx);
    }

    void erase(const T &x) {
        insert(root, x, MAX_LOG, -1, -1);
    }

    Node *find(const T &x) {
        return find(root, x, MAX_LOG);
    }

    int count(const T &x) {
        Node *nx =  find(x);
        return nx == nullptr ? 0 : nx->exist;
    }

    pair<T, Node *> min_element() {
        assert(root->exist > 0);
        return kth_element(0);
    }

    pair<T, Node *> max_element() {
        assert(root->exist > 0);
        return kth_element(root->exist - 1);
    }

    pair<T, Node *> kth_element(int k) {
        assert(0 <= k and k < root->exist);
        return kth_element(root, k, MAX_LOG);
    }

    int count_less(const T &x) {
        return count_less(root, x, MAX_LOG);
    }

    int count_leq(const T &x) {
        return count_less(root, x+1, MAX_LOG);
    }

    int count_greater(const T &x) {
        return root->exist - count_less(root, x+1, MAX_LOG);
    }

    int count_geq(const T &x) {
        return root->exist - count_less(root, x, MAX_LOG);
    }

    void operate_xor(const T &x) {
        lazy ^= x;
    }

private:

    void insert(Node *n_cur, const T &x, int depth, int delta, int idx) {
        if (depth == -1) {
            n_cur->exist += delta;
            if (idx != -1) n_cur->accept.emplace_back(idx);
        }
        else {
            bool nxt_01 = (lazy >> depth & 1) ^ (x >> depth & 1);
            if(!n_cur->nxt[nxt_01]) n_cur->nxt[nxt_01] = new Node();
            insert(n_cur->nxt[nxt_01], x, depth - 1, delta, idx);
            n_cur->exist += delta;
        }
    }

    Node *find(Node *n_cur, const T &x, int depth) {
        if (depth == -1) {
            return n_cur;
        }
        else {
            bool nxt_01 = (lazy >> depth & 1) ^ (x >> depth & 1);
            Node *n_nxt = n_cur->nxt[nxt_01];
            return n_nxt == nullptr ? n_nxt : find(n_nxt, x, depth - 1);
        }
    }

    pair<T, Node *> kth_element(Node *n_cur, int k, int depth) {
        if (depth == -1) {
            return make_pair(0, n_cur);
        }
        else {
            bool nxt_01 = lazy >> depth & 1;
            int comp = n_cur->nxt[nxt_01] == nullptr ?
                0 : n_cur->nxt[nxt_01]->exist;
            if (comp <= k) {
                pair<T, Node*> ret = kth_element(
                    n_cur->nxt[nxt_01 ^ 1], k - comp, depth - 1);
                ret.first |= T(1) << depth;
                return ret;
            }
            else{
                return kth_element(n_cur->nxt[nxt_01], k, depth - 1);
            }
        }
    }

    int count_less(Node *n_cur, const T &x, int depth) {
        if (depth == -1) return 0;
        int ret = 0;
        bool nxt_01 = lazy >> depth & 1;
        bool xi = x >> depth & 1;
        if (xi and n_cur->nxt[nxt_01] != nullptr)
            ret += n_cur->nxt[nxt_01]->exist;
        if (n_cur->nxt[nxt_01 ^ xi] != nullptr)
            ret += count_less(n_cur->nxt[nxt_01 ^ xi], x, depth - 1);
        return ret;
    }
};


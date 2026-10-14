template<class T, bool find_min = true>
struct CartesianTree {
    int root;
    vector<int> par, left, right;
    stack<int> st;

    static constexpr bool upd(const T& cur, const T& pre) {
        return find_min ? cur < pre : cur > pre;
    }

    CartesianTree(const vector<T> &a):
    root(0), par(a.size(), -1), left(a.size(), -1), right(a.size(), -1) {
        for (int i = 0; i < (int)a.size(); i++) {
            int last_popped = -1;
            while (!st.empty() and upd(a[i], a[st.top()])) {
                last_popped = st.top();
                st.pop();
            }
            if (last_popped != -1) {
                par[last_popped] = i;
                left[i] = last_popped;
            }
            if (st.empty()) {
                root = i;
            }
            else{
                par[i] = st.top();
                right[st.top()] = i;
            }
            st.emplace(i);
        }
    }
};

template <int char_size, int base>
struct CompressedSuffixTree {
    struct Node {
        int l, r;
        int start, end;
        int accept_idx;
        array<int, char_size> nxt;
        Node(int l_, int r_, int start_, int end_, int accept_idx_ = -1):
            l(l_), r(r_), start(start_), end(end_), accept_idx(accept_idx_) {
            nxt.fill(-1);
        }
        inline int size() { return end - start; }
        // for debug only
        // 対応する文字列は s[ sa[l]+start : sa[l]+end )
        inline string get_substr(const string &s, const vc<int> &sa) {
            assert(start < end);
            assert(sa[l]+end <= s.size());
            return start == -1 ? "^" : s.substr(sa[l] + start, end - start);
        }
    };

    vector<Node> nodes;
    int root;
    CompressedSuffixTree(const string &s): root(0) {
        int n = s.size();
        nodes.emplace_back(0, n, -1, 0); // root
        if (n == 1) {
            nodes[0].nxt[s[0]-base] = 1;
            nodes.emplace_back(0, 1, 0, 1, 0);
            return;
        }
        auto sa = atcoder::suffix_array(s);
        auto la = atcoder::lcp_array(s,sa);
        CartesianTree<int, true> ct(la);
        auto dfs = [&] (auto&& dfs, int l, int r, int u, int nid) -> void {
            // u = argmin la[l : r-1)
            if (la[u] > nodes[nid].end) {
                nodes[nid].nxt[s[sa[l]+nodes[nid].end] - base] = nodes.size();
                nodes.emplace_back(l, r, nodes[nid].end, la[u]);
                nid = nodes.size() - 1;
            }
            if (ct.left[u] != -1) {
                dfs(dfs, l, u+1, ct.left[u], nid);
            }
            else{
                // leaf node for u (left)
                if (sa[u]+nodes[nid].end == n) nodes[nid].accept_idx = u;
                else {
                    nodes[nid].nxt[s[sa[u]+nodes[nid].end] - base] = nodes.size();
                    nodes.emplace_back(u, u+1, nodes[nid].end, n-sa[u], u);
                }
            }
            if (ct.right[u] != -1) {
                dfs(dfs, u+1, r, ct.right[u], nid);
            }
            else{
                // leaf node for u+1 (right)
                if (sa[u+1]+nodes[nid].end == n) nodes[nid].accept_idx = u+1;
                else {
                    nodes[nid].nxt[s[sa[u+1]+nodes[nid].end] - base] = nodes.size();
                    nodes.emplace_back(u+1, u+2, nodes[nid].end, n-sa[u+1], u+1);
                }
            }
        };
        dfs(dfs, 0, n, ct.root, 0);
    }
};

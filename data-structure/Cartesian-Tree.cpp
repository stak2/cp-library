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

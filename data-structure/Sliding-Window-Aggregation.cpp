template <typename T, T (*fold)(const T&, const T&), T (*e)()>
struct SlidingWindowAggregation {
    stack<T> elem_front, elem_back;
    stack<T> prefix_front, prefix_back;
    T sum_front, sum_back;

    SlidingWindowAggregation(): sum_front(e()), sum_back(e()) {}

private:
    void push_frnont(const T &x) {
        elem_front.push(x);
        prefix_front.push(sum_front = fold(x, sum_front));
    }

    void push_back(const T &x) {
        elem_back.push(x);
        prefix_back.push(sum_back = fold(sum_back, x));
    }

    void transfer() {
        while(!elem_back.empty()) {
            push_front(elem_back.top());
            elem_back.pop();
        }
        prefix_back.clear();
        sum_back = e();
    }

public:
    bool empty() const { return elem_front.empty() and elem_back.empty(); }
    size_t size() const { return elem_front.size() + elem_back.size(); }

    void push(const T &x) {
        push_back(x);
    }

    void pop() {
        assert(!empty());
        if (elem_front.empty()) transfer();
        elem_front().pop();
        prefix_front().pop();
        sum_front = elem_front().empty() ? e() : prefix_front().top();
    }

    T fold_all() const {
        return fold(sum_front, sum_back);
    }
};



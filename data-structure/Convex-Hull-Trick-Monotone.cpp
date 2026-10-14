template <typename T, bool minimize = true>
struct ConvexHullTrickMonotone{
    struct Line {
        T a, b;
        Line(T a_, T b_): a(a_), b(b_) {}
        inline T operator()(const T x) const { return a * x + b; }
    };

    deque<Line> dq;

    ConvexHullTrickMonotone() = default;

    static constexpr bool check(const Line &f1, const Line &f2, const Line &f3) {
        // f1.a > f2.a > f3.a
        if constexpr (is_integral<T>::value) {
            return div_floor(f2.b - f1.b, f1.a - f2.a) >= div_floor(f3.b - f2.b, f2.a - f3.a);
        }
        else {
            return (f2.b - f1.b) / (f1.a - f2.a) >= (f3.b - f2.b) / (f2.a - f3.a);
        }
        // return (f2.b - f1.b) * (f2.a - f3.a) >= (f3.b - f2.b) * (f1.a - f2.a);
    }

    void push_back(const Line &f) {
        if (!dq.empty() and dq.back().a == f.a) {
            if (dq.back().b <= f.b) return;
            dq.pop_back();
        }

        while (dq.size() >= 2 and
               check(dq[dq.size() - 2], dq[dq.size() - 1], f)) dq.pop_back();
        dq.emplace_back(f);
    }

    void push_front(const Line &f) {
        if (!dq.empty() and dq.front().a == f.a) {
            if (dq.front().b <= f.b) return;
            dq.pop_front();
        }

        while (dq.size() >= 2 and
               check(f, dq[0], dq[1])) dq.pop_front();
        dq.emplace_front(f);
    }

    void add_line(T a, T b) {
        if constexpr (!minimize) a = -a, b = -b;
        if (dq.size() and dq.back().a >= a) push_back(Line{a, b});
        else push_front(Line{a, b});
    }

    T query(const T x) const {
        int ng = -1, ok = dq.size() - 1;
        while (ng + 1 < ok) {
            int mid = (ok + ng) / 2;
            if (dq[mid](x) < dq[mid + 1](x)) ok = mid;
            else ng = mid;
        }
        if constexpr (minimize) return dq[ok](x);
        else return -dq[ok](x);
    }

    // x: increasing
    T query_monotone_inc(const T x) {
        while (dq.size() >= 2 and dq[0](x) >= dq[1](x))
            dq.pop_front();
        if constexpr (minimize) return dq.front()(x);
        else return -dq.front()(x);
    }

    // x: decreasing
    T query_monotone_dec(const T x) {
        while (dq.size() >= 2 and dq[dq.size() - 2](x) >= dq[dq.size() - 1](x))
            dq.pop_back();
        if constexpr (minimize) return dq.back()(x);
        else return -dq.back()(x);
    }
};

// 0 は追加されない
// weight の一番小さいものを削除
// cmp(a,b) == true <=> a > b
// a は b より重要か？
template <int B, typename T, bool (*cmp)(const T&, const T&)>
struct WeightedF2Basis {
    using BS = bitset<B>;
    array<BS, B> basis, expr;
    array<T, B> weight;
    WeightedF2Basis() {
        for (int i = 0; i < B; i++) basis[i] = expr[i] = 0;
        // for (int i = 0; i < B; i++) weight[i] = -1;
    }

    bool add(BS x, T w_x) {
        BS expr_x;
        for (int i = 0; i < B; i++) {
            if (x.test(i) and basis[i].test(i)) {
                x ^= basis[i];
                expr_x ^= expr[i];
            }
        }
        if (x.any()) {
            int d = -1;
            for (int i = 0; i < B; i++) {
                if (x.test(i)) {
                    d = i;
                    break;
                }
            }
            // assert(d != -1);
            expr_x.set(d);
            for (int i = 0; i < B; i++) {
                if (basis[i].test(d)) {
                    basis[i] ^= x;
                    expr[i] ^= expr_x;
                }
            }
            basis[d] = x;
            expr[d] = expr_x;
            weight[d] = w_x;
            return true;
        }
        else {
            int d = -1;
            for (int i = 0; i < B; i++) {
                if (expr_x.test(i)) {
                    // assert(weight[i] != -1);
                    if (d == -1 or cmp(weight[d], weight[i])) d = i;
                }
            }
            if (d == -1) return false;
            weight[d] = w_x;
            expr_x.reset(d);
            for (int i = 0; i < B; i++) {
                if (expr[i].test(d)) expr[i] ^= expr_x;
            }
            return false;
        }
    }

    pair<bool, BS> get_expr(BS x) const {
        BS expr_x;
        for (int i = 0; i < B; i++) {
            if (x.test(i) and basis[i].test(i)) {
                x ^= basis[i];
                expr_x ^= expr[i];
            }
        }
        return make_pair(x.none(), expr_x);
    }
};

bool cmp(const int& a, const int& b) { return a > b; }


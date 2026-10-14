#include "atcoder/modint.hpp"
#include "atcoder/convolution.hpp"

template <typename mint>
struct Polynomial : vector<mint> {
    using vector<mint>::vector;
    using poly = Polynomial<mint>;

    void shrink() {
        while(!this->empty() and this->back() == mint(0)) this->pop_back();
    }

    poly rev() const {
        poly res(*this);
        reverse(res.begin(), res.end());
        return res;
    }

    poly pre(int sz) const {
        poly res(this->begin(), this->begin() + min(this->size(), (size_t)sz));
        if (res.size() < (size_t)sz) res.resize(sz);
        return res;
    }

    poly &operator>>=(int sz) {
        if (this->size() <= (size_t)sz) {
            this->clear();
            return *this;
        }
        this->erase(this->begin(), this->begin() + (size_t)sz);
        return *this;
    }

    poly &operator<<=(int sz) {
        this->insert(this->begin(), sz, mint::raw(0));
        return *this;
    }

    poly &operator+=(const mint &s) {
        if (this->empty()) this->resize(1);
        (*this)[0] += s;
        return *this;
    }

    poly &operator+=(const poly &p) {
        if (this->size() < p.size()) this->resize(p.size());
        for (size_t i = 0; i < p.size(); i++) (*this)[i] += p[i];
        return *this;
    }

    poly &operator-=(const mint &s) {
        if (this->empty()) this->resize(1);
        (*this)[0] -= s;
        return *this;
    }

    poly &operator-=(const poly &p) {
        if (this->size() < p.size()) this->resize(p.size());
        for (size_t i = 0; i < p.size(); i++) (*this)[i] -= p[i];
        return *this;
    }

    poly &operator*=(const mint &s) {
        for (size_t i = 0; i < this->size(); i++) (*this)[i] *= s;
        return *this;
    }

    poly operator*=(const poly &p) {
        if (this->empty() or p.empty()) {
            this->clear();
            return *this;
        }
        vector<mint> prod = atcoder::convolution(*this, p);
        return *this = poly(prod.begin(), prod.end());
    }

    poly &operator/=(const mint &s) {
        mint s_inv = s.inv();
        for (size_t i = 0; i < this->size(); i++) (*this)[i] *= s_inv;
        return *this;
    }

    poly operator/=(const poly &p) {
        if (this->size() < p.size()) {
            this->clear();
            return *this;
        }
        size_t q_sz = this->size() - p.size() + 1;
        return *this = (this->rev().pre(q_sz) * p.rev().inv(q_sz)).pre(q_sz).rev();
    }

    poly operator%=(const poly &p) {
        *this -= (*this / p) * p;
        shrink();
        assert(this->size() < p.size());
        return *this;
    }

    poly operator+(const mint &s) const { return poly(*this) += s; }
    poly operator+(const poly &p) const { return poly(*this) += p; }
    poly operator-(const mint &s) const { return poly(*this) -= s; }
    poly operator-(const poly &p) const { return poly(*this) -= p; }
    poly operator*(const mint &s) const { return poly(*this) *= s; }
    poly operator*(const poly &p) const { return poly(*this) *= p; }
    poly operator/(const mint &s) const { return poly(*this) /= s; }
    poly operator/(const poly &p) const { return poly(*this) /= p; }
    poly operator%(const poly &p) const { return poly(*this) %= p; }
    poly operator>>(int s) const { return poly(*this) >>= s; }
    poly operator<<(int s) const { return poly(*this) <<= s; }
    poly operator-() const {
        poly res(this->size());
        for (size_t i = 0; i < this->size(); i++) res[i] = -(*this)[i];
        return res;
    }

    poly dot(const poly &p) const {
        poly res(min(this->size(), p.size()));
        for (size_t i = 0; i < res.size(); i++) res[i] = (*this)[i] * p[i];
        return res;
    }

    mint eval(mint x) const {
        mint res = 0, pw = 1;
        for (auto &c : *this) res += c * pw, pw *= x;
        return res;
    }

    void ntt() {
        atcoder::internal::butterfly(*this);
    }

    void intt() {
        atcoder::internal::butterfly_inv(*this);
        mint sz_inv = mint::raw(this->size()).inv();
        for (auto &c : *this) c *= sz_inv;
    }

    poly inv(int deg = -1) const {
        assert(!this->empty() and (*this)[0] != 0);
        if (deg == -1) deg = (int)this->size();
        poly res(deg);
        res[0] = (*this)[0].inv();
        for (int d = 1; d < deg; d <<= 1) {
            poly f(2 * d), g(2 * d);
            for (int j = 0; j < min((int)this->size(), 2 * d); j++) f[j] = (*this)[j];
            for (int j = 0; j < d; j++) g[j] = res[j];
            // atcoder::internal::butterfly(f);
            f.ntt();
            // atcoder::internal::butterfly(g);
            g.ntt();
            for (int j = 0; j < 2 * d; j++) f[j] *= g[j];
            // atcoder::internal::butterfly_inv(f);
            f.intt();
            for (int j = 0; j < d; j++) f[j] = 0;
            // atcoder::internal::butterfly(f);
            f.ntt();
            for (int j = 0; j < 2 * d; j++) f[j] *= g[j];
            // atcoder::internal::butterfly_inv(f);
            f.intt();
            for (int j = d; j < min(2 * d, deg); j++) res[j] = -f[j];
        }
        return res.pre(deg);
    }

    poly differentiate() const {
        const size_t n = this->size();
        poly res(n == 0 ? 0 : n - 1);
        mint coef = 1;
        for (size_t i = 1; i < n; i++) {
            res[i - 1] = (*this)[i] * coef;
            coef += 1;
        }
        return res;
    }

    poly integrate() const {
        const size_t n = this->size();
        poly res(n + 1);
        res[0] = mint::raw(0);
        if (n > 0) res[1] = mint(1);
        int mod = mint::mod();
        for (size_t i = 2; i <= n; i++) res[i] = (-res[mod % i]) * (mod / i);
        for (size_t i = 0; i < n; i++) res[i + 1] *= (*this)[i];
        return res;
    }

    poly log(int deg = -1) const {
        assert(!this->empty() and (*this)[0] == 1);
        if (deg == -1) deg = (int)this->size();
        return (this->differentiate() * this->inv(deg)).pre(deg - 1).integrate();
    }

    poly exp(int deg = -1) const {
        assert(this->empty() or (*this)[0] == mint(0));
        if (deg == -1) deg = (int)this->size();

        vector<mint> inv;
        inv.reserve(deg + 1);
        inv.emplace_back(mint::raw(0));
        inv.emplace_back(mint::raw(1));
        poly b{1, 1 < (int)this->size() ? (*this)[1] : 0};
        poly c{1}, z1, z2{1, 1};
        int mod = mint::mod();
        for (int d = 2; d < deg ; d <<= 1) {
            poly y = b;
            y.resize(2 * d);
            // atcoder::internal::butterfly(y);
            y.ntt();
            z1 = z2;
            poly z(d);
            for (int i = 0; i < d; i++) z[i] = y[i] * z1[i];
            // atcoder::internal::butterfly_inv(z);
            z.intt();
            fill(z.begin(), z.begin() + d / 2, mint::raw(0));
            // atcoder::internal::butterfly(z);
            z.ntt();
            for (int i = 0; i < d; i++) z[i] *= -z1[i];
            // atcoder::internal::butterfly_inv(z);
            z.intt();
            c.insert(c.end(), z.begin() + d / 2, z.end());
            z2 = c;
            z2.resize(2 * d);
            // atcoder::internal::butterfly(z2);
            z2.ntt();
            poly x(this->begin(), this->begin() + min<int>(this->size(), d));
            x.resize(d);
            {
                x.erase(x.begin());
                mint coef = 1;
                for (int i = 0; i < d-1; i++) x[i] *= coef, coef += 1;
            }
            x.emplace_back(mint::raw(0));
            // atcoder::internal::butterfly(x);
            x.ntt();
            for (int i = 0; i < d; i++) x[i] *= y[i];
            // atcoder::internal::butterfly_inv(x);
            x.intt();
            x -= b.differentiate();
            x.resize(2 * d);
            for (int i = 0; i < d - 1; i++) x[d + i] = x[i], x[i] = mint::raw(0);
            // atcoder::internal::butterfly(x);
            x.ntt();
            for (int i = 0; i < 2 * d; i++) x[i] *= z2[i];
            // atcoder::internal::butterfly_inv(x);
            x.intt();
            x.pop_back();
            {
                int sz;
                while((sz = (int)inv.size()) <= (int)x.size()) {
                    inv.emplace_back((-inv[mod % sz]) * (mod / sz));
                }
                x.insert(x.begin(), mint::raw(0));
                for (size_t i = 1; i < x.size(); i++) x[i] *= inv[i];
            }
            for (int i = d; i < min((int)this->size(), 2 * d); i++) x[i] += (*this)[i];
            fill(x.begin(), x.begin() + d, mint::raw(0));
            // atcoder::internal::butterfly(x);
            x.ntt();
            for (int i = 0; i < 2 * d; i++) x[i] *= y[i];
            // atcoder::internal::butterfly_inv(x);
            x.intt();
            b.insert(b.end(), x.begin() + d, x.end());
        }
        return b.pre(deg);
    }

    poly pow(long long k, int deg = -1) const {
        const int n = (int)this->size();
        if (deg == -1) deg = n;
        if (k == 0) {
            poly res(deg);
            if (deg > 0) res[0] = 1;
            return res;
        }
        int l = 0;
        while (l < n and (*this)[l] == 0){
            l++;
            if (l >= (deg + k - 1) / k) return poly(deg, mint::raw(0));
        }
        if (l == n) return poly(deg, mint::raw(0));
        mint rev = (*this)[l].inv();
        poly res = (((*this * rev) >> l).log(deg) * k).exp(deg);
        res *= (*this)[l].pow(k);
        res = (res << (k * l)).pre(deg);
        if ((int)res.size() < deg) res.resize(deg, mint::raw(0));
        return res;
    }
};

using mint = atcoder::modint998244353;
using poly = Polynomial<mint>;


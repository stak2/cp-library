#include <cstring>
#include <unistd.h>

namespace fastio {

#ifndef FAST_IO
#define FAST_IO
#endif

struct Pre {
    char num[10000][4];
    constexpr Pre() : num() {
        for (int i = 0; i < 10000; i++) {
            int n = i;
            for (int j = 3; j >= 0; j--) {
                num[i][j] = n % 10 | '0';
                n /= 10;
            }
        }
    }
} constexpr pre;

static constexpr int BUF_SZ = 1 << 18;
char ibuf[BUF_SZ], obuf[BUF_SZ], outs[100];
int pil = 0, pir = 0, por = 0, outi;

inline void load() {
    memcpy(ibuf, ibuf + pil, pir - pil);
    pir = pir - pil + fread(ibuf + pir - pil, 1, BUF_SZ - pir + pil, stdin);
    pil = 0;
    if (pir < BUF_SZ) ibuf[pir++] = '\n';
}
inline void flush() { fwrite(obuf, 1, por, stdout), por = 0; }

void input(char &c) {
    do{
        if (pil + 1 > pir) load();
        c = ibuf[pil++];
    } while(isspace(c));
}
void input(string &x) {
    x.clear();
    char c;
    do {
        if (pil + 1 > pir) load();
        c = ibuf[pil++];
    } while (isspace(c));
    do {
        x += c;
        if (pil == pir) load();
        c = ibuf[pil++];
    } while (!isspace(c));
}

template <typename T>
void input_integer(T &x) {
    if (pil + 100 > pir) load();
    char c;
    do { c = ibuf[pil++]; } while (c < '-');
    bool minus = 0;
    if constexpr (is_signed<T>::value || is_same_v<T, __int128>) {
        if (c == '-') { minus = 1, c = ibuf[pil++]; }
    }
    x = 0;
    while (c >= '0') { x = x * 10 + (c & 15), c = ibuf[pil++]; }
    if constexpr (is_signed<T>::value || is_same_v<T, __int128>) {
        if (minus) { x = -x; }
    }
}

template <typename T>
void input_real(T &x) { string s; input(s); x = stod(s); }

void input(int &x) { input_integer(x); }
void input(long long &x) { input_integer(x); }
void input(__int128 &x) { input_integer(x); }
void input(uint32_t &x) { input_integer(x); }
void input(uint64_t &x) { input_integer(x); }
void input(unsigned __int128 &x) { input_integer(x); }
void input(double &x) { input_real(x); }

template <class T>
void input(vector<T> &x) { for (auto &d : x) input(d); }
template <typename T, size_t N = 0>
void input(array<T, N> &x) { for (auto &d : x) input(d); }
template <class T, class U>
void input(pair<T, U> &p) { input(p.first), input(p.second); }
template <size_t N = 0, typename T>
void input_tuple(T &t) {
    if constexpr (N < std::tuple_size<T>::value) {
        auto &x = std::get<N>(t);
        input(x);
        input_tuple<N + 1>(t);
    }
}
template <class... T>
void input(tuple<T...> &tpl) { input_tuple(tpl); }

void in() {}
template <class H, class... T>
void in(H &h, T &... t) { input(h), in(t...); }

void output(const char c) {
    if (por == BUF_SZ) flush();
    obuf[por++] = c;
}
void output(const string &s) { for (char c : s) output(c); }

template <typename T>
void output_integer(T x) {
    if (por > BUF_SZ - 100) flush();
    if (x < 0) { obuf[por++] = '-', x = -x; }
    for (outi = 96; x >= 10000; outi -= 4) {
        memcpy(outs + outi, pre.num[x % 10000], 4);
        x /= 10000;
    }
    if (x >= 1000) {
        memcpy(obuf + por, pre.num[x], 4);
        por += 4;
    }
    else if (x >= 100) {
        memcpy(obuf + por, pre.num[x] + 1, 3);
        por += 3;
    }
    else if(x >= 10) {
        int q = (x * 103) >> 10;
        obuf[por] = q | '0';
        obuf[por + 1] = (x - q * 10) | '0';
        por += 2;
    }
    else {
        obuf[por++] = x | '0';
    }
    memcpy(obuf + por, outs + outi + 4, 96 - outi);
    por += 96 - outi;
}

template <typename T>
void output_real(T x) {
    ostringstream oss;
    oss << fixed << setprecision(15) << double(x);
    string s = oss.str();
    output(s);
}

void output(int x) { output_integer(x); }
void output(long long x) { output_integer(x); }
void output(__int128 x) { output_integer(x); }
void output(uint32_t x) { output_integer(x); }
void output(uint64_t x) { output_integer(x); }
void output(unsigned __int128 x) { output_integer(x); }
void output(double x) { output_real(x); }
void output(long double x) { output_real(x); }

template <class T>
void output(const vector<T> &val) {
    size_t n = val.size();
    for (size_t i = 0; i < n; i++) {
        if (i) output(' ');
        output(val[i]);
    }
}

template <class T, class U>
void output(const pair<T, U> &val) {
    output(val.first); output(' '); output(val.second);
}
template <size_t N = 0, typename T>
void output_tuple(const T &t) {
    if constexpr (N < std::tuple_size<T>::value) {
        if constexpr (N > 0) { output(' '); }
        const auto x = std::get<N>(t);
        output(x);
        output_tuple<N+1>(t);
    }
}
template <class... T>
void output(tuple<T...> &tpl) { output_tuple(tpl); }
template <class T, size_t N>
void output(const array<T, N> &val) {
    size_t n = val.size();
    for (size_t i = 0; i < n; i++) {
        if (i) output(' ');
        output(val[i]);
    }
}

void out() { output('\n'); }
template <class H, class... T, char sep = ' '>
void out(H &&h, T &&... t) {
    output(h);
    if (sizeof...(T)) output(sep);
    out(t...);
}

void outr() {}
template <class H, class... T>
void outr(H &&h, T &&... t) {
    output(h);
    outr(t...);
}

void __attribute__((destructor)) _d() { flush(); }
} // namespace fastio

using fastio::in;
using fastio::out;


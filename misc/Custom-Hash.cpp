#include <chrono>
#include <functional>
using namespace std;

struct custom_hash {
    const unsigned long long FIXED_RANDOM = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    static unsigned long long splitmix64(unsigned long long x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    template <typename T>
    unsigned long long hash_combine(unsigned long long hr, const T &x) const noexcept {
        return hr ^ (std::hash<T>()(x) + (hr << 11) + (hr >> 13));
    }
    template <typename T, typename U>
    unsigned long long operator()(const pair<T, U> &p) const noexcept {
        unsigned long long x = hash_combine(std::hash<T>()(p.first), p.second);
        return splitmix64(x + FIXED_RANDOM);
    }

    unsigned long long operator()(const unsigned long long x) const noexcept {
        return splitmix64(x + FIXED_RANDOM);
    }
};

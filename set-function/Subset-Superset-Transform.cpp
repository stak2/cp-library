// b[s] = \sum_{t ⊆ s} a[t]
template <typename T>
void subset_zeta(vector<T> &a) {
    int n = a.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int s = 0; s < n; s++) {
            if (s & i) a[s] += a[s ^ i];
        }
    }
}

// a[s] = \sum_{t ⊆ s} b[t]
template <typename T>
void subset_mobius(vector<T> &a) {
    int n = a.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int s = 0; s < n; s++) {
            if (s & i) a[s] -= a[s ^ i];
        }
    }
}

// b[s] = \sum_{t ⊇ s} a[t]
template <typename T>
void superset_zeta(vector<T> &a) {
    int n = a.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int s = 0; s < n; s++) {
            if (!(s & i)) a[s] += a[s ^ i];
        }
    }
}

// a[s] = \sum_{t ⊇ s} b[t]
template <typename T>
void superset_mobius(vector<T> &a) {
    int n = a.size();
    assert((n & (n-1)) == 0);
    for (int i = 1; i < n; i <<= 1) {
        for (int s = 0; s < n; s++) {
            if (!(s & i)) a[s] -= a[s ^ i];
        }
    }
}

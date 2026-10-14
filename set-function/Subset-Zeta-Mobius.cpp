template <typename T>
void subset_zeta(vector<T> &a) {
    int n = a.size();
    for (int i = 1; i < n; i <<= 1) {
        for (int j = 0; j < n; j += 1 << i) {
            for (int k = 0; k < i; k++) {
                a[j + k + i] += a[j + k];
            }
        }
    }
}
template <typename T>
void subset_mobius(vector<T> &a) {
    int n = a.size();
    for (int i = 1; i < n; i <<= 1) {
        for (int j = 0; j < n; j += 1 << i) {
            for (int k = 0; k < i; k++) {
                a[j + k + i] -= a[j + k];
            }
        }
    }
}

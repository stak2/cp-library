mint fact[MX], finv[MX], inv[MX];

mint com(int n, int k) {
    if(n < 0 or k < 0 or n < k) return 0;
    return fact[n] * finv[k] * finv[n-k];
}

void cominit() {
    fact[0] = fact[1] = 1;
    finv[0] = finv[1] = 1;
    inv[1] = 1;
    for (int i = 2; i < MX; i++) {
        fact[i] = fact[i-1] * i;
        inv[i] = mod - inv[mod%i] * (mod/i);
        finv[i] = finv[i-1] * inv[i];
    }
}

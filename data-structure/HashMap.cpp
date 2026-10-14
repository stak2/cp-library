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
        return hr ^ ((*this)(x) + (hr << 11) + (hr >> 13));
    }
    template <typename T, typename U>
    unsigned long long operator()(const pair<T, U> &p) const noexcept {
        return hash_combine((*this)(p.first), p.second);
    }

    unsigned long long operator()(const unsigned long long x) const noexcept {
        return splitmix64(x + FIXED_RANDOM);
    }
    unsigned long long operator()(const string& x) const noexcept {
        return splitmix64(std::hash<string>()(x) + FIXED_RANDOM);
    }
};

template <typename K, typename V>
struct HashMap {
    class iterator;
private:
    custom_hash ch;
    unsigned cap, mask;
    vector<pair<K, V>> data;
    vector<bool> used;

    constexpr uint64_t hash(K x) const { return ch(x) & mask; }

    void extend() {
        vector<pair<K, V>> dat;
        dat.reserve(used.size()/2-cap);
        for (int i = 0; i < (int)used.size(); i++) if (used[i]) dat.emplace_back(data[i]);
        build(2 * dat.size());
        for (const auto& [k, v] : dat) (*this)[k] = v;
    }
public:
    HashMap (unsigned n = 0) { build(n); }
    void build(unsigned n) {
        unsigned k = 8;
        while (k < n*2) k *= 2;
        cap = k/2, mask = k-1;
        data.resize(k), used.assign(k, false);
    }

    // do not change size
    void clear() {
        used.assign(used.size(), false);
        cap = (mask+1)/2;
    }
    int size() const { return used.size()/2 - cap; }

    int index(const K& k) const {
        int i = 0;
        for (i = hash(k); used[i] and data[i].first != k; i = (i+1)&mask);
        return i;
    }

    V& operator[](const K& k) {
        if (cap == 0) extend();
        int i = index(k);
        if (!used[i]) { used[i] = true, data[i].first = k, data[i].second = V{}, cap--; }
        return data[i].second;
    }

    iterator find(const K& k) const {
        int i = index(k);
        return used[i] ? iterator(i, this) : this->end();
    }

    bool contains(const K& k) const {
        int i = index(k);
        return used[i] and data[i].first == k;
    }

    void erase(const K& k) {
        int i = index(k);
        if(used[i]) { used[i] = false, data[i].first = 0, data[i].second = V{}, cap++; }
    }

    // execute f(key, val) for every entry
    template <typename F>
    void enumerate_all(F f) const {
        for (int i = 0; i < (int)used.size(); i++) if (used[i]) f(data[i].first, data[i].second);
    }

    class iterator {
        unsigned i;
        HashMap *p;
    public:
        explicit constexpr iterator(): i(0), p(nullptr) {}
        explicit constexpr iterator(unsigned i_, HashMap *p_): i(i_), p(p_) {}
        explicit constexpr iterator(unsigned i_, const HashMap *p_):
            i(i_), p(const_cast<HashMap*>(p_)) {}
        friend void swap(iterator &a, iterator &b) { swap(a.i, b.i); swap(a.p, b.p); }
        friend bool operator==(const iterator &a, const iterator &b) { return a.i == b.i; }
        friend bool operator!=(const iterator &a, const iterator &b) { return a.i != b.i; }
        const pair<K, V>& operator*() const { return const_cast<const HashMap*>(p)->data[i]; }
        pair<K, V>& operator*() { return p->data[i]; }
        pair<K, V>* operator->() const { return &(p->data[i]); }
        iterator& operator++() {
            assert(i != p->used.size());
            do {
                i++;
                if (i == p->used.size()) break;
                if (p->used[i]) break;
            } while(true);
            return *this;
        }
        iterator operator++(int) {
            iterator it(*this);
            ++(*this);
            return it;
        }
        iterator& operator--() {
            do {
                i--;
                if (p->used[i]) break;
                assert(i != 0);
            } while(true);
            return *this;
        }
        iterator operator--(int) {
            iterator it(*this);
            --(*this);
            return it;
        }
    };
    iterator begin() const {
        unsigned h = 0;
        while (h != used.size()) {
            if (used[h]) break;
            h++;
        }
        return iterator(h, this);
    }
    iterator end() const { return iterator(this->used.size(), this); }
    friend iterator begin(const HashMap &h) { return h.begin(); }
    friend iterator end(const HashMap &h) { return h.end(); }
};

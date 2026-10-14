#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

namespace fast_mo {

enum class Ordering { automatic, snake, hilbert };
enum class Sorting { automatic, comparison, radix };
enum class SnakeBuild { automatic, independent, shared };
struct Options {
    Ordering ordering = Ordering::automatic;
    int block_size = 0; // 0: N * sqrt(3 / (2 Q)), adjusted for left/right costs
    bool polish = true;
    bool polish_all_candidates = false;
    int polish_span = 8, polish_passes = 2;
    Sorting sorting = Sorting::automatic;
    SnakeBuild snake_build = SnakeBuild::automatic;
    bool relocate = false; // Optional bounded single-query insertion search.
    bool hilbert_lookup = true;
    unsigned left_cost = 1, right_cost = 1;
};
struct Query { int l, r, id; }; // half-open [l, r)
struct Statistics {
    std::uint64_t left_moves = 0, right_moves = 0;
    std::uint64_t weighted_moves = 0;
    int block_size = 0; // 0 if Hilbert was selected
};

// Build once, execute with inlineable callbacks. No type erasure in the hot loop.
class Mo {
    int n_;
    std::vector<Query> queries_, ordered_;
    Statistics stats_;
    bool built_ = false;
    static void radix_sort(std::vector<std::uint64_t>& values,
                           std::vector<std::uint64_t>& buffer, std::uint64_t maximum) {
        buffer.resize(values.size());
        // Stable LSD radix sort; 8KB histogram, at most six passes.
        for (unsigned shift = 0; maximum >> shift; shift += 11) {
            std::array<unsigned, 2048> positions{};
            for (auto v : values) ++positions[(v >> shift) & 2047];
            unsigned total = 0;
            for (auto& p : positions) { unsigned count = p; p = total; total += count; }
            for (auto v : values) buffer[positions[(v >> shift) & 2047]++] = v;
            values.swap(buffer);
            if (shift >= 55) break; // Never shift a 64-bit integer by >=64.
        }
    }
    static std::uint64_t hilbert_bitwise(unsigned x, unsigned y, int bits) {
        std::uint64_t result = 0;
        for (unsigned s = 1u << (bits - 1); s; s >>= 1) {
            unsigned rx = !!(x & s), ry = !!(y & s);
            result += std::uint64_t(s) * s * ((3 * rx) ^ ry);
            if (!ry) {
                if (rx) { x = ~x; y = ~y; }
                std::swap(x, y);
            }
        }
        return result;
    }
    static std::uint64_t hilbert(unsigned x, unsigned y, int bits) {
        // Each entry emits eight order bits and the swap/complement parity
        // for the remaining coordinates after processing four levels.
        static constexpr auto table = [] {
            std::array<unsigned short, 256> result{};
            for (unsigned input = 0; input < 256; ++input) {
                unsigned a = input >> 4, b = input & 15, order = 0, state = 0;
                for (unsigned s = 8; s; s >>= 1) {
                    unsigned rx = !!(a & s), ry = !!(b & s);
                    order += s * s * ((3 * rx) ^ ry);
                    if (!ry) {
                        if (rx) { a = ~a; b = ~b; state ^= 2; }
                        unsigned saved = a; a = b; b = saved; state ^= 1;
                    }
                }
                result[input] = static_cast<unsigned short>((order << 2) | state);
            }
            return result;
        }();
        std::uint64_t result = 0;
        const int remainder = bits & 3;
        for (int bit = bits - 1; bit >= bits - remainder; --bit) {
            unsigned rx = (x >> bit) & 1, ry = (y >> bit) & 1;
            result = (result << 2) | ((3 * rx) ^ ry);
            if (!ry) { if (rx) { x = ~x; y = ~y; } std::swap(x, y); }
        }
        for (int shift = bits - remainder - 4; shift >= 0; shift -= 4) {
            unsigned value = table[(((x >> shift) & 15) << 4) | ((y >> shift) & 15)];
            result = (result << 8) | (value >> 2);
            if (value & 2) { x = ~x; y = ~y; }
            if (value & 1) std::swap(x, y);
        }
        return result;
    }
    static std::uint64_t distance(const Query& a, const Query& b, const Options& o) {
        return std::uint64_t(std::abs(a.l - b.l)) * o.left_cost
             + std::uint64_t(std::abs(a.r - b.r)) * o.right_cost;
    }
    std::uint64_t cost(const std::vector<Query>& a, const Options& o) const {
        Query previous{0, 0, -1};
        std::uint64_t total = 0;
        for (const auto& q : a) { total += distance(previous, q, o); previous = q; }
        return total;
    }
    static void polish(std::vector<Query>& a, const Options& o) {
        // Bounded local 2-opt. Symmetric distance makes each reversal O(1)
        // to assess. O(Q * passes) work for the bounded span list.
        const Query origin{0, 0, -1};
        const int size = int(a.size());
        for (int pass = 0; pass < o.polish_passes; ++pass) {
            for (int i = 0; i + 1 < size; ++i) {
                for (int span : {2, 3, 4, 6, 8, 12, 16, 24, 32}) {
                    if (span > o.polish_span) break;
                    int j = i + span - 1;
                    if (j >= size) break;
                    const Query& before = i ? a[i - 1] : origin;
                    auto old_cost = distance(before, a[i], o);
                    auto new_cost = distance(before, a[j], o);
                    if (j + 1 < size) {
                        old_cost += distance(a[j], a[j + 1], o);
                        new_cost += distance(a[i], a[j + 1], o);
                    }
                    if (new_cost < old_cost) std::reverse(a.begin() + i, a.begin() + j + 1);
                }
            }
        }
        if (o.relocate) {
            for (int pass = 0; pass < o.polish_passes; ++pass) {
                for (int i = 0; i + 1 < size; ++i) {
                    const Query& before = i ? a[i - 1] : origin;
                    int chosen = i;
                    std::uint64_t saving = 0;
                    for (int j = i + 1; j < size && j - i < o.polish_span; ++j) {
                        auto old_cost = distance(before, a[i], o) + distance(a[i], a[i + 1], o);
                        auto new_cost = distance(before, a[i + 1], o) + distance(a[j], a[i], o);
                        if (j + 1 < size) {
                            old_cost += distance(a[j], a[j + 1], o);
                            new_cost += distance(a[i], a[j + 1], o);
                        }
                        if (old_cost > new_cost && old_cost - new_cost > saving) {
                            saving = old_cost - new_cost; chosen = j;
                        }
                    }
                    if (chosen != i)
                        std::rotate(a.begin() + i, a.begin() + i + 1, a.begin() + chosen + 1);
                }
            }
        }
    }
public:
    explicit Mo(int n, int reserve_queries = 0) : n_(n) {
        assert(n >= 0 && reserve_queries >= 0);
        queries_.reserve(reserve_queries);
    }
    int add_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        const int id = int(queries_.size());
        queries_.push_back({l, r, id});
        built_ = false;
        return id;
    }
    void build(Options options = {}) {
        assert(options.left_cost > 0 && options.right_cost > 0);
        assert(options.polish_span >= 2 && options.polish_span <= 32 && options.polish_passes >= 0);
        ordered_.clear(); stats_ = {};
        built_ = true;
        if (queries_.empty()) return;
        struct Key { std::uint64_t value; int id; };
        std::vector<Key> keys;
        std::vector<std::uint64_t> packed(queries_.size()), buffer;
        std::vector<Query> candidate(queries_.size());
        unsigned id_bits = 0;
        while ((std::uint64_t(1) << id_bits) < queries_.size()) ++id_bits;
        const std::uint64_t id_mask = (std::uint64_t(1) << id_bits) - 1;
        std::uint64_t best = UINT64_MAX;
        auto accept = [&](int block) {
            if (options.polish && options.polish_all_candidates) polish(candidate, options);
            auto current = cost(candidate, options);
            if (current < best) {
                best = current; ordered_ = candidate; stats_.block_size = block;
            }
        };
        std::vector<int> right_order, query_blocks;
        std::vector<std::size_t> boundaries, positions;
        auto shared_snake = [&](int block) {
            const std::size_t buckets = std::size_t(n_ / block) + 1;
            // Small Q / huge coordinate ranges must not allocate O(N) memory.
            if (buckets > queries_.size() * 4 + 4096) return false;
            if (right_order.empty()) {
                right_order.resize(queries_.size());
                if (std::uint64_t(n_) <= queries_.size() * 4 + 4096) {
                    positions.assign(std::size_t(n_) + 1, 0);
                    for (const auto& q : queries_) ++positions[q.r];
                    std::size_t total = 0;
                    for (auto& p : positions) { auto count = p; p = total; total += count; }
                    // Registration order is the tie breaker for equal r.
                    for (const auto& q : queries_) right_order[positions[q.r]++] = q.id;
                } else {
                    std::uint64_t maximum = 0;
                    for (std::size_t i = 0; i < queries_.size(); ++i) {
                        packed[i] = (std::uint64_t(queries_[i].r) << id_bits) | i;
                        maximum = std::max(maximum, packed[i]);
                    }
                    if (options.sorting == Sorting::comparison) std::sort(packed.begin(), packed.end());
                    else radix_sort(packed, buffer, maximum);
                    for (std::size_t i = 0; i < packed.size(); ++i) right_order[i] = int(packed[i] & id_mask);
                }
                query_blocks.resize(queries_.size());
            }
            boundaries.assign(buckets + 1, 0);
            for (std::size_t i = 0; i < queries_.size(); ++i) {
                int b = queries_[i].l / block;
                query_blocks[i] = b; ++boundaries[std::size_t(b) + 1];
            }
            for (std::size_t b = 0; b < buckets; ++b) boundaries[b + 1] += boundaries[b];
            positions.resize(buckets);
            for (std::size_t b = 0; b < buckets; ++b)
                positions[b] = boundaries[b + (b & 1)];
            for (int id : right_order) {
                int b = query_blocks[id];
                std::size_t index = (b & 1) ? --positions[b] : positions[b]++;
                candidate[index] = queries_[id];
            }
            // Descending r must still retain ascending id for equal r. This
            // makes shared construction identical to the independent sorter.
            for (std::size_t b = 1; b < buckets; b += 2) {
                for (std::size_t begin = boundaries[b]; begin < boundaries[b + 1];) {
                    std::size_t end = begin + 1;
                    while (end < boundaries[b + 1] && candidate[end].r == candidate[begin].r) ++end;
                    std::reverse(candidate.begin() + begin, candidate.begin() + end);
                    begin = end;
                }
            }
            accept(block);
            return true;
        };
        auto consider = [&](int block) {
            bool shared = options.snake_build == SnakeBuild::shared ||
                (options.snake_build == SnakeBuild::automatic &&
                 options.sorting == Sorting::automatic && queries_.size() >= 2048);
            if (block && shared && shared_snake(block)) return;
            int bits = 1;
            while ((std::uint64_t(1) << bits) <= unsigned(n_)) ++bits;
            auto query_key = [&](std::size_t i) {
                const auto& q = queries_[i];
                std::uint64_t key;
                if (block) {
                    unsigned b = unsigned(q.l / block);
                    key = std::uint64_t(b) * (std::uint64_t(n_) + 1)
                        + ((b & 1) ? unsigned(n_ - q.r) : unsigned(q.r));
                } else key = options.hilbert_lookup ? hilbert(unsigned(q.l), unsigned(q.r), bits)
                                                    : hilbert_bitwise(unsigned(q.l), unsigned(q.r), bits);
                return key;
            };
            std::uint64_t maximum = 0;
            bool fits = true;
            for (std::size_t i = 0; i < queries_.size(); ++i) {
                auto key = query_key(i);
                if (id_bits && (key >> (64 - id_bits))) fits = false;
                packed[i] = (key << id_bits) | i;
                maximum = std::max(maximum, packed[i]);
            }
            if (fits) {
                if (options.sorting == Sorting::radix ||
                    (options.sorting == Sorting::automatic && packed.size() >= 2048))
                    radix_sort(packed, buffer, maximum);
                else std::sort(packed.begin(), packed.end());
                for (std::size_t i = 0; i < packed.size(); ++i)
                    candidate[i] = queries_[std::size_t(packed[i] & id_mask)];
            } else {
                // Full int coordinates can exceed the packing budget.
                keys.resize(queries_.size());
                for (std::size_t i = 0; i < keys.size(); ++i) keys[i] = {query_key(i), int(i)};
                std::sort(keys.begin(), keys.end(), [](const Key& a, const Key& b) {
                    return a.value < b.value || (a.value == b.value && a.id < b.id);
                });
                for (std::size_t i = 0; i < keys.size(); ++i) candidate[i] = queries_[keys[i].id];
            }
            accept(block);
        };
        int base = options.block_size;
        if (base <= 0) {
            double desired = n_ / std::sqrt(queries_.size() * (2.0 / 3.0))
                           * std::sqrt(double(options.right_cost) / options.left_cost);
            base = std::max(1, int(std::min(double(std::max(1, n_)), desired)));
        }
        if (options.ordering == Ordering::snake) consider(base);
        else if (options.ordering == Ordering::hilbert) consider(0);
        else {
            std::vector<int> blocks;
            for (double multiplier : {0.75, 1.0, 1.25, 1.5})
                blocks.push_back(std::max(1, int(std::min(double(std::max(1, n_)), base * multiplier))));
            std::sort(blocks.begin(), blocks.end());
            blocks.erase(std::unique(blocks.begin(), blocks.end()), blocks.end());
            for (int block : blocks) consider(block);
            consider(0);
        }
        if (options.polish && !options.polish_all_candidates) polish(ordered_, options);
        Query previous{0, 0, -1};
        for (const auto& q : ordered_) {
            stats_.left_moves += unsigned(std::abs(q.l - previous.l));
            stats_.right_moves += unsigned(std::abs(q.r - previous.r));
            previous = q;
        }
        stats_.weighted_moves = stats_.left_moves * options.left_cost + stats_.right_moves * options.right_cost;
    }
    const Statistics& statistics() const { assert(built_); return stats_; }
    const std::vector<Query>& order() const { assert(built_); return ordered_; }
    template<class AddLeft, class AddRight, class EraseLeft, class EraseRight, class Answer>
    void run(AddLeft add_left, AddRight add_right,
             EraseLeft erase_left, EraseRight erase_right, Answer answer) const {
        assert(built_);
        int l = 0, r = 0;
        for (const Query& q : ordered_) {
            const int target_l = q.l, target_r = q.r, id = q.id;
            // Expand first: every callback observes a valid interval.
            while (l > target_l) add_left(--l);
            while (r < target_r) add_right(r++);
            while (l < target_l) erase_left(l++);
            while (r > target_r) erase_right(--r);
            answer(id);
        }
    }
    template<class Add, class Erase, class Answer>
    void run(Add add, Erase erase, Answer answer) const {
        run(add, add, erase, erase, answer);
    }
};
} // namespace fast_mo
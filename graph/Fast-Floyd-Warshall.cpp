#pragma once
// Cache-blocked Floyd-Warshall; requires no negative cycles.
// Design reference: https://github.com/windows-server-2003/quick_floyd_warshall
// SIMD kernels and runtime dispatch are independently implemented.
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)
#define SF_X86 1
#include <immintrin.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#else
#define SF_X86 0
#endif
#if defined(__GNUC__) || defined(__clang__)
#define SF_TARGET(x) __attribute__((target(x), noinline))
#elif defined(_MSC_VER)
#define SF_TARGET(x) __declspec(noinline)
#else
#define SF_TARGET(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define SF_INLINE(x) __attribute__((target(x), always_inline)) inline
#elif defined(_MSC_VER)
#define SF_INLINE(x) __forceinline
#else
#define SF_INLINE(x) inline
#endif

namespace simd_floyd {
enum class Backend { Auto, Scalar, AVX2, AVX512 };
inline const char* name(Backend b) {
    switch (b) { case Backend::Auto: return "Auto"; case Backend::Scalar: return "Scalar";
    case Backend::AVX2: return "AVX2"; default: return "AVX512"; }
}
inline bool supported(Backend b) {
    if (b == Backend::Auto || b == Backend::Scalar) return true;
#if SF_X86 && (defined(__GNUC__) || defined(__clang__)) && !defined(_MSC_VER)
    __builtin_cpu_init();
    return b == Backend::AVX2 ? bool(__builtin_cpu_supports("avx2"))
                             : bool(__builtin_cpu_supports("avx512f"));
#elif SF_X86 && defined(_MSC_VER)
    int r[4]; __cpuid(r, 0); const int max_leaf = r[0];
    if (max_leaf < 7) return false;
    __cpuidex(r, 1, 0);
    if ((r[2] & (1 << 27)) == 0 || (r[2] & (1 << 28)) == 0) return false;
    const auto x = _xgetbv(0);
    if ((x & 6) != 6) return false;
    __cpuidex(r, 7, 0);
    return b == Backend::AVX2 ? (r[1] & (1 << 5)) != 0
                             : (x & 0xe6) == 0xe6 && (r[1] & (1 << 16)) != 0;
#else
    return false;
#endif
}
inline Backend select(Backend b) {
    if (b == Backend::Auto) {
        if (supported(Backend::AVX512)) return Backend::AVX512;
        if (supported(Backend::AVX2)) return Backend::AVX2;
        return Backend::Scalar;
    }
    if (!supported(b)) throw std::runtime_error("Requested SIMD backend is unavailable");
    return b;
}


struct RunStats {
    double packing_ms = 0, processing_ms = 0, unpacking_ms = 0, total_ms = 0;
    bool bounded_arithmetic = false;
};
namespace detail {
#if SF_X86
template<class T> struct Ops256 {
    using V = __m256i;
    static constexpr int width = 32 / sizeof(T);
    static SF_INLINE("avx2") V load(const T* p) { return _mm256_loadu_si256(reinterpret_cast<const V*>(p)); }
    static SF_INLINE("avx2") void store(T* p, V v) { _mm256_storeu_si256(reinterpret_cast<V*>(p),v); }
    static SF_INLINE("avx2") V broadcast(T a) {
        if constexpr(sizeof(T)==4) return _mm256_set1_epi32(a);
        else return _mm256_set1_epi64x(a);
    }
    static SF_INLINE("avx2") V add(V a, V b) {
        if constexpr(sizeof(T)==4) return _mm256_add_epi32(a,b);
        else return _mm256_add_epi64(a,b);
    }
    static SF_INLINE("avx2") V maximum(V a, V b) {
        if constexpr(sizeof(T)==4) return _mm256_max_epi32(a,b);
        else return _mm256_blendv_epi8(a,b,_mm256_cmpgt_epi64(b,a));
    }
    static SF_INLINE("avx2") V sum_checked(V a, V b, V inf, V upper) {
        V v=add(a,b);
        if constexpr(sizeof(T)==4) {
            v=_mm256_min_epi32(v,upper);
            const V mask=_mm256_or_si256(_mm256_cmpeq_epi32(a,inf),_mm256_cmpeq_epi32(b,inf));
            return _mm256_blendv_epi8(v,inf,mask);
        } else {
            v=_mm256_blendv_epi8(v,upper,_mm256_cmpgt_epi64(v,upper));
            const V mask=_mm256_or_si256(_mm256_cmpeq_epi64(a,inf),_mm256_cmpeq_epi64(b,inf));
            return _mm256_blendv_epi8(v,inf,mask);
        }
    }
    static SF_INLINE("avx2") void update(T* dst, V candidate) {
        if constexpr(sizeof(T)==8) {
            const V old=load(dst);
            _mm256_maskstore_epi64(reinterpret_cast<long long*>(dst),_mm256_cmpgt_epi64(candidate,old),candidate);
        } else store(dst,maximum(load(dst),candidate));
    }
};
template<class T> struct Ops512 {
    using V = __m512i;
    static constexpr int width = 64 / sizeof(T);
    static SF_INLINE("avx512f") V load(const T* p) { return _mm512_loadu_si512(reinterpret_cast<const V*>(p)); }
    static SF_INLINE("avx512f") void store(T* p, V v) { _mm512_storeu_si512(reinterpret_cast<V*>(p),v); }
    static SF_INLINE("avx512f") V broadcast(T a) {
        if constexpr(sizeof(T)==4) return _mm512_set1_epi32(a);
        else return _mm512_set1_epi64(a);
    }
    static SF_INLINE("avx512f") V add(V a, V b) {
        if constexpr(sizeof(T)==4) return _mm512_add_epi32(a,b);
        else return _mm512_add_epi64(a,b);
    }
    static SF_INLINE("avx512f") V maximum(V a, V b) {
        if constexpr(sizeof(T)==4) return _mm512_max_epi32(a,b);
        else return _mm512_max_epi64(a,b);
    }
    static SF_INLINE("avx512f") V sum_checked(V a, V b, V inf, V upper) {
        V v=add(a,b);
        if constexpr(sizeof(T)==4) {
            v=_mm512_min_epi32(v,upper);
            const auto mask=_mm512_cmpeq_epi32_mask(a,inf) | _mm512_cmpeq_epi32_mask(b,inf);
            return _mm512_mask_mov_epi32(v,static_cast<__mmask16>(mask),inf);
        } else {
            v=_mm512_min_epi64(v,upper);
            const auto mask=_mm512_cmpeq_epi64_mask(a,inf) | _mm512_cmpeq_epi64_mask(b,inf);
            return _mm512_mask_mov_epi64(v,static_cast<__mmask8>(mask),inf);
        }
    }
    static SF_INLINE("avx512f") void update(T* dst, V candidate) {
        const V old=load(dst);
        if constexpr(sizeof(T)==4)
            _mm512_mask_storeu_epi32(dst,_mm512_cmpgt_epi32_mask(candidate,old),candidate);
        else _mm512_mask_storeu_epi64(dst,_mm512_cmpgt_epi64_mask(candidate,old),candidate);
    }
};
#endif
} // namespace detail

// No negative cycles. Tile, Rows, and KUnroll are optional tuning parameters.
template<class T = std::int32_t, std::size_t Tile = 64, int Rows = (sizeof(T)==8 ? 4 : 2), int KUnroll = 4,
         int Columns = (sizeof(T)==8 ? 0 : (Tile>=64 ? 4 : Tile>=32 ? 2 : 1)), bool Recursive = true>
class Matrix {
    static_assert(std::is_integral_v<T> && std::is_signed_v<T> && (sizeof(T)==4 || sizeof(T)==8),
                  "Matrix requires a signed 32-bit or 64-bit integer type");
    static_assert(Tile >= 16 && Tile <= 256 && Tile % 16 == 0, "Tile must be a multiple of 16, in [16,256]");
    static_assert((Rows==1 || Rows==2 || Rows==4) && (KUnroll==1 || KUnroll==2 || KUnroll==4), "Invalid unroll shape");
    static_assert(Columns==0 || Columns==1 || Columns==2 || Columns==4, "Invalid register column count");
    static_assert(Columns==0 || Tile % ((Columns ? Columns : 1)*64/sizeof(T))==0,
                  "Tile must be divisible by the register column span");
public:
    using distance_type = T;
    static constexpr T INF = std::numeric_limits<T>::max()/2;
    static constexpr std::size_t lanes = 64/sizeof(T);
    explicit Matrix(std::size_t n) : n_(n) {
        if(n > std::numeric_limits<std::size_t>::max() - Tile)
            throw std::length_error("Matrix size overflow");
        stride_ = (n+lanes-1)/lanes*lanes;
        if(stride_ && n > std::vector<T>().max_size()/stride_)
            throw std::length_error("Matrix size overflow");
        d_.assign(n*stride_,INF);
        for(std::size_t i=0;i<n;++i) (*this)(i,i)=0;
    }
    std::size_t size() const { return n_; }
    T& operator()(std::size_t i,std::size_t j) { return d_[i*stride_+j]; }
    T operator()(std::size_t i,std::size_t j) const { return d_[i*stride_+j]; }
    void add_edge(std::size_t i,std::size_t j,T w) {
        if(i>=n_ || j>=n_) throw std::out_of_range("Vertex index");
        if(w<=-INF || w>=INF) throw std::out_of_range("Edge weight must be in (-INF,INF)");
        (*this)(i,j)=std::min((*this)(i,j),w);
    }
    // force_checked is useful for validation/benchmarking of arithmetic overhead.
    Backend run(Backend backend=Backend::Auto, RunStats* stats=nullptr, bool force_checked=false) {
        using Clock=std::chrono::steady_clock;
        const auto start=Clock::now();
        backend=select(backend);
        T max_abs=0;
        for(std::size_t i=0;i<n_;++i) {
            if((*this)(i,i)!=0) throw std::invalid_argument("Diagonal must be zero; negative cycles are unsupported");
            for(std::size_t j=0;j<n_;++j) {
                const T v=(*this)(i,j);
                if(v<-INF || v>INF) throw std::out_of_range("Distance outside supported range");
                if(v!=INF) max_abs=std::max(max_abs,v<0 ? -v : v);
            }
        }
        // Shortest simple paths have <= n-1 edges. Keeping all such sums below INF/4
        // separates true distances from sentinel paths, eliminating inner-loop masks.
        const bool bounded=!force_checked && (n_==0 ||
            (n_ <= static_cast<std::uint64_t>(INF/4-1) &&
             max_abs <= (INF/4-1)/static_cast<T>(n_)));
        const T sentinel = bounded ? INF/2 : INF;
        const std::size_t blocks=(n_+Tile-1)/Tile, padded=blocks*Tile;
        if(padded && padded>(std::vector<T>().max_size()-lanes)/padded)
            throw std::length_error("Packed matrix size overflow");
        // Extra space allows a 64-byte aligned pointer without an allocator dependency.
        std::vector<T> buffer(padded*padded+lanes,-sentinel);
        T* packed=reinterpret_cast<T*>((reinterpret_cast<std::uintptr_t>(buffer.data())+63)&~std::uintptr_t(63));
        std::vector<T*> starts(blocks*blocks);
        std::size_t span=1; while(span<blocks) span*=2;
        for(std::size_t i=0;i<blocks*blocks;++i) starts[i]=packed+i*Tile*Tile;
        auto tile=[&](std::size_t i,std::size_t j) { return starts[i*blocks+j]; };
        for(std::size_t bi=0;bi<blocks;++bi) for(std::size_t bj=0;bj<blocks;++bj) {
            T* dst=tile(bi,bj);
            const auto nr=std::min(Tile,n_-bi*Tile), nc=std::min(Tile,n_-bj*Tile);
            for(std::size_t i=0;i<nr;++i) for(std::size_t j=0;j<nc;++j) {
                T v=(*this)(bi*Tile+i,bj*Tile+j);
                dst[i*Tile+j]=v==INF ? -sentinel : -v;
            }
        }
        const auto packed_at=Clock::now();
        if(backend==Backend::Scalar) {
            if(bounded) schedule_scalar<false>(starts.data(),blocks);
            else schedule_scalar<true>(starts.data(),blocks);
        }
#if SF_X86
        else if(backend==Backend::AVX2) {
            if(bounded) schedule256<false>(starts.data(),blocks);
            else schedule256<true>(starts.data(),blocks);
        } else {
            if(bounded) schedule512<false>(starts.data(),blocks);
            else schedule512<true>(starts.data(),blocks);
        }
#endif
        const auto processed_at=Clock::now();
        for(std::size_t bi=0;bi<blocks;++bi) for(std::size_t bj=0;bj<blocks;++bj) {
            const T* src=tile(bi,bj);
            const auto nr=std::min(Tile,n_-bi*Tile), nc=std::min(Tile,n_-bj*Tile);
            for(std::size_t i=0;i<nr;++i) for(std::size_t j=0;j<nc;++j) {
                const T v=src[i*Tile+j];
                (*this)(bi*Tile+i,bj*Tile+j)=bounded && v<=-INF/4 ? INF : -v;
            }
        }
        const auto done=Clock::now();
        if(stats) {
            stats->packing_ms=std::chrono::duration<double,std::milli>(packed_at-start).count();
            stats->processing_ms=std::chrono::duration<double,std::milli>(processed_at-packed_at).count();
            stats->unpacking_ms=std::chrono::duration<double,std::milli>(done-processed_at).count();
            stats->total_ms=std::chrono::duration<double,std::milli>(done-start).count();
            stats->bounded_arithmetic=bounded;
        }
        return backend;
    }
    // Kept for source compatibility; not a guaranteed negative-cycle detector.
    bool has_negative_cycle() const {
        for(std::size_t i=0;i<n_;++i) if((*this)(i,i)<0) return true;
        return false;
    }
private:
    std::size_t n_,stride_;
    std::vector<T> d_;
    template<bool Checked> static void scalar_tile(T* dst,const T* left,const T* right) {
        for(std::size_t k=0;k<Tile;++k) for(std::size_t i=0;i<Tile;++i) {
            const T a=left[i*Tile+k];
            if constexpr(Checked) { if(a==-INF) continue; }
            for(std::size_t j=0;j<Tile;++j) {
                const T b=right[k*Tile+j];
                if constexpr(Checked) { if(b==-INF) continue; }
                T v=a+b;
                if constexpr(Checked) v=std::min(v,INF);
                dst[i*Tile+j]=std::max(dst[i*Tile+j],v);
            }
        }
    }

    template<bool Checked> static void recursivescalar(T** data,std::size_t blocks,
            std::size_t i,std::size_t k,std::size_t j,std::size_t span) {
        if(i>=blocks || k>=blocks || j>=blocks) return;
        if(span==1) {
            T* dst=data[i*blocks+j];
            const T* left=data[i*blocks+k];
            const T* right=data[k*blocks+j];
            if(i!=k && j!=k) scalar_tile<Checked>(dst,left,right);
            else scalar_tile<Checked>(dst,left,right);
            return;
        }
        const auto half=span/2;
        // Visit the pivot quadrant first, then its row/column, then the remainder.
        // Completing both pivot halves preserves Floyd's dependency order.
        for(std::size_t pk=0;pk<2;++pk) for(std::size_t pi=0;pi<2;++pi)
            for(std::size_t pj=0;pj<2;++pj)
                recursivescalar<Checked>(data,blocks,i+((pk+pi)%2)*half,k+pk*half,
                    j+((pk+pj)%2)*half,half);
    }
    template<bool Checked> static void schedule_scalar(T** data,std::size_t blocks) {
        if constexpr(Recursive) {
            std::size_t span=1; while(span<blocks) span*=2;
            recursivescalar<Checked>(data,blocks,0,0,0,span);
            return;
        } else {
        auto tile=[&](std::size_t i,std::size_t j) { return data[i*blocks+j]; };
        for(std::size_t k=0;k<blocks;++k) {
            T* diagonal=tile(k,k);
            scalar_tile<Checked>(diagonal,diagonal,diagonal);
            for(std::size_t j=0;j<blocks;++j) if(j!=k)
                scalar_tile<Checked>(tile(k,j),diagonal,tile(k,j));
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                scalar_tile<Checked>(tile(i,k),tile(i,k),diagonal);
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                for(std::size_t j=0;j<blocks;++j) if(j!=k)
                    scalar_tile<Checked>(tile(i,j),tile(i,k),tile(k,j));
        }
            }
    }
#if SF_X86

    template<bool Checked,bool Disjoint>
    static SF_INLINE("avx2") void tile256(T* dst,const T* left,const T* right) {
        using O=detail::Ops256<T>; using V=typename O::V;
        constexpr int KU=Disjoint ? KUnroll : 1;
        const V inf=O::broadcast(-INF),upper=O::broadcast(INF);
        if constexpr(Disjoint && Columns==0) {
            if constexpr(KUnroll==1) {
                for(std::size_t k=0;k<Tile;k+=1) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            O::update(dst+(i+0)*Tile+j,v0);
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            O::update(dst+(i+1)*Tile+j,v0);
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            O::update(dst+(i+2)*Tile+j,v0);
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            O::update(dst+(i+3)*Tile+j,v0);
                        }
                    }
                }
            }
            if constexpr(KUnroll==2) {
                for(std::size_t k=0;k<Tile;k+=2) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a01; if constexpr(Rows>0) a01=O::broadcast(left[(i+0)*Tile+k+1]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a11; if constexpr(Rows>1) a11=O::broadcast(left[(i+1)*Tile+k+1]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a21; if constexpr(Rows>2) a21=O::broadcast(left[(i+2)*Tile+k+1]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    V a31; if constexpr(Rows>3) a31=O::broadcast(left[(i+3)*Tile+k+1]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        const V b1=O::load(right+(k+1)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a01,b1,inf,upper); else v1=O::add(a01,b1);
                            O::update(dst+(i+0)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a11,b1,inf,upper); else v1=O::add(a11,b1);
                            O::update(dst+(i+1)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a21,b1,inf,upper); else v1=O::add(a21,b1);
                            O::update(dst+(i+2)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a31,b1,inf,upper); else v1=O::add(a31,b1);
                            O::update(dst+(i+3)*Tile+j,O::maximum(v0,v1));
                        }
                    }
                }
            }
            if constexpr(KUnroll==4) {
                for(std::size_t k=0;k<Tile;k+=4) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a01; if constexpr(Rows>0) a01=O::broadcast(left[(i+0)*Tile+k+1]);
                    V a02; if constexpr(Rows>0) a02=O::broadcast(left[(i+0)*Tile+k+2]);
                    V a03; if constexpr(Rows>0) a03=O::broadcast(left[(i+0)*Tile+k+3]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a11; if constexpr(Rows>1) a11=O::broadcast(left[(i+1)*Tile+k+1]);
                    V a12; if constexpr(Rows>1) a12=O::broadcast(left[(i+1)*Tile+k+2]);
                    V a13; if constexpr(Rows>1) a13=O::broadcast(left[(i+1)*Tile+k+3]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a21; if constexpr(Rows>2) a21=O::broadcast(left[(i+2)*Tile+k+1]);
                    V a22; if constexpr(Rows>2) a22=O::broadcast(left[(i+2)*Tile+k+2]);
                    V a23; if constexpr(Rows>2) a23=O::broadcast(left[(i+2)*Tile+k+3]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    V a31; if constexpr(Rows>3) a31=O::broadcast(left[(i+3)*Tile+k+1]);
                    V a32; if constexpr(Rows>3) a32=O::broadcast(left[(i+3)*Tile+k+2]);
                    V a33; if constexpr(Rows>3) a33=O::broadcast(left[(i+3)*Tile+k+3]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        const V b1=O::load(right+(k+1)*Tile+j);
                        const V b2=O::load(right+(k+2)*Tile+j);
                        const V b3=O::load(right+(k+3)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a01,b1,inf,upper); else v1=O::add(a01,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a02,b2,inf,upper); else v2=O::add(a02,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a03,b3,inf,upper); else v3=O::add(a03,b3);
                            O::update(dst+(i+0)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a11,b1,inf,upper); else v1=O::add(a11,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a12,b2,inf,upper); else v2=O::add(a12,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a13,b3,inf,upper); else v3=O::add(a13,b3);
                            O::update(dst+(i+1)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a21,b1,inf,upper); else v1=O::add(a21,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a22,b2,inf,upper); else v2=O::add(a22,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a23,b3,inf,upper); else v3=O::add(a23,b3);
                            O::update(dst+(i+2)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a31,b1,inf,upper); else v1=O::add(a31,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a32,b2,inf,upper); else v2=O::add(a32,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a33,b3,inf,upper); else v3=O::add(a33,b3);
                            O::update(dst+(i+3)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                    }
                }
            }
            return;
        }

        else if constexpr(Disjoint && Columns>0) {
            for(std::size_t i=0;i<Tile;i+=Rows) for(std::size_t j=0;j<Tile;j+=Columns*O::width) {
                V acc[Rows][Columns];
                for(int r=0;r<Rows;++r) for(int c=0;c<Columns;++c)
                    acc[r][c]=O::load(dst+(i+r)*Tile+j+c*O::width);
                for(std::size_t k=0;k<Tile;++k) {
                    V rhs[Columns];
                    for(int c=0;c<Columns;++c) rhs[c]=O::load(right+k*Tile+j+c*O::width);
                    for(int r=0;r<Rows;++r) {
                        const V a=O::broadcast(left[(i+r)*Tile+k]);
                        for(int c=0;c<Columns;++c) {
                            V candidate;
                            if constexpr(Checked) candidate=O::sum_checked(a,rhs[c],inf,upper);
                            else candidate=O::add(a,rhs[c]);
                            acc[r][c]=O::maximum(acc[r][c],candidate);
                        }
                    }
                }
                for(int r=0;r<Rows;++r) for(int c=0;c<Columns;++c)
                    O::store(dst+(i+r)*Tile+j+c*O::width,acc[r][c]);
            }
            return;
        } else {
        for(std::size_t k=0;k<Tile;k+=KU) for(std::size_t i=0;i<Tile;i+=Rows) {
            V coef[Rows][KU];
            for(int r=0;r<Rows;++r) for(int t=0;t<KU;++t)
                coef[r][t]=O::broadcast(left[(i+r)*Tile+k+t]);
            for(std::size_t j=0;j<Tile;j+=O::width) {
                V rhs[KU];
                for(int t=0;t<KU;++t) rhs[t]=O::load(right+(k+t)*Tile+j);
                for(int r=0;r<Rows;++r) {
                    V candidate;
                    if constexpr(Checked) candidate=O::sum_checked(coef[r][0],rhs[0],inf,upper);
                    else candidate=O::add(coef[r][0],rhs[0]);
                    for(int t=1;t<KU;++t) {
                        V next;
                        if constexpr(Checked) next=O::sum_checked(coef[r][t],rhs[t],inf,upper);
                        else next=O::add(coef[r][t],rhs[t]);
                        candidate=O::maximum(candidate,next);
                    }
                    O::update(dst+(i+r)*Tile+j,candidate);
                }
            }
        }
            }
    }

    template<bool Checked> SF_TARGET("avx2") void recursive256(T** data,std::size_t blocks,
            std::size_t i,std::size_t k,std::size_t j,std::size_t span) {
        if(i>=blocks || k>=blocks || j>=blocks) return;
        if(span==1) {
            T* dst=data[i*blocks+j];
            const T* left=data[i*blocks+k];
            const T* right=data[k*blocks+j];
            if(i!=k && j!=k) tile256<Checked,true>(dst,left,right);
            else tile256<Checked,false>(dst,left,right);
            return;
        }
        const auto half=span/2;
        // Visit the pivot quadrant first, then its row/column, then the remainder.
        // Completing both pivot halves preserves Floyd's dependency order.
        for(std::size_t pk=0;pk<2;++pk) for(std::size_t pi=0;pi<2;++pi)
            for(std::size_t pj=0;pj<2;++pj)
                recursive256<Checked>(data,blocks,i+((pk+pi)%2)*half,k+pk*half,
                    j+((pk+pj)%2)*half,half);
    }
    template<bool Checked> SF_TARGET("avx2") void schedule256(T** data,std::size_t blocks) {
        if constexpr(Recursive) {
            std::size_t span=1; while(span<blocks) span*=2;
            recursive256<Checked>(data,blocks,0,0,0,span);
            return;
        } else {
        auto tile=[&](std::size_t i,std::size_t j) { return data[i*blocks+j]; };
        for(std::size_t k=0;k<blocks;++k) {
            T* diagonal=tile(k,k);
            tile256<Checked,false>(diagonal,diagonal,diagonal);
            for(std::size_t j=0;j<blocks;++j) if(j!=k)
                tile256<Checked,false>(tile(k,j),diagonal,tile(k,j));
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                tile256<Checked,false>(tile(i,k),tile(i,k),diagonal);
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                for(std::size_t j=0;j<blocks;++j) if(j!=k)
                    tile256<Checked,true>(tile(i,j),tile(i,k),tile(k,j));
        }
            }
    }

    template<bool Checked,bool Disjoint>
    static SF_INLINE("avx512f") void tile512(T* dst,const T* left,const T* right) {
        using O=detail::Ops512<T>; using V=typename O::V;
        constexpr int KU=Disjoint ? KUnroll : 1;
        const V inf=O::broadcast(-INF),upper=O::broadcast(INF);
        if constexpr(Disjoint && Columns==0) {
            if constexpr(KUnroll==1) {
                for(std::size_t k=0;k<Tile;k+=1) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            O::update(dst+(i+0)*Tile+j,v0);
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            O::update(dst+(i+1)*Tile+j,v0);
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            O::update(dst+(i+2)*Tile+j,v0);
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            O::update(dst+(i+3)*Tile+j,v0);
                        }
                    }
                }
            }
            if constexpr(KUnroll==2) {
                for(std::size_t k=0;k<Tile;k+=2) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a01; if constexpr(Rows>0) a01=O::broadcast(left[(i+0)*Tile+k+1]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a11; if constexpr(Rows>1) a11=O::broadcast(left[(i+1)*Tile+k+1]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a21; if constexpr(Rows>2) a21=O::broadcast(left[(i+2)*Tile+k+1]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    V a31; if constexpr(Rows>3) a31=O::broadcast(left[(i+3)*Tile+k+1]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        const V b1=O::load(right+(k+1)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a01,b1,inf,upper); else v1=O::add(a01,b1);
                            O::update(dst+(i+0)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a11,b1,inf,upper); else v1=O::add(a11,b1);
                            O::update(dst+(i+1)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a21,b1,inf,upper); else v1=O::add(a21,b1);
                            O::update(dst+(i+2)*Tile+j,O::maximum(v0,v1));
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a31,b1,inf,upper); else v1=O::add(a31,b1);
                            O::update(dst+(i+3)*Tile+j,O::maximum(v0,v1));
                        }
                    }
                }
            }
            if constexpr(KUnroll==4) {
                for(std::size_t k=0;k<Tile;k+=4) for(std::size_t i=0;i<Tile;i+=Rows) {
                    V a00; if constexpr(Rows>0) a00=O::broadcast(left[(i+0)*Tile+k+0]);
                    V a01; if constexpr(Rows>0) a01=O::broadcast(left[(i+0)*Tile+k+1]);
                    V a02; if constexpr(Rows>0) a02=O::broadcast(left[(i+0)*Tile+k+2]);
                    V a03; if constexpr(Rows>0) a03=O::broadcast(left[(i+0)*Tile+k+3]);
                    V a10; if constexpr(Rows>1) a10=O::broadcast(left[(i+1)*Tile+k+0]);
                    V a11; if constexpr(Rows>1) a11=O::broadcast(left[(i+1)*Tile+k+1]);
                    V a12; if constexpr(Rows>1) a12=O::broadcast(left[(i+1)*Tile+k+2]);
                    V a13; if constexpr(Rows>1) a13=O::broadcast(left[(i+1)*Tile+k+3]);
                    V a20; if constexpr(Rows>2) a20=O::broadcast(left[(i+2)*Tile+k+0]);
                    V a21; if constexpr(Rows>2) a21=O::broadcast(left[(i+2)*Tile+k+1]);
                    V a22; if constexpr(Rows>2) a22=O::broadcast(left[(i+2)*Tile+k+2]);
                    V a23; if constexpr(Rows>2) a23=O::broadcast(left[(i+2)*Tile+k+3]);
                    V a30; if constexpr(Rows>3) a30=O::broadcast(left[(i+3)*Tile+k+0]);
                    V a31; if constexpr(Rows>3) a31=O::broadcast(left[(i+3)*Tile+k+1]);
                    V a32; if constexpr(Rows>3) a32=O::broadcast(left[(i+3)*Tile+k+2]);
                    V a33; if constexpr(Rows>3) a33=O::broadcast(left[(i+3)*Tile+k+3]);
                    for(std::size_t j=0;j<Tile;j+=O::width) {
                        const V b0=O::load(right+(k+0)*Tile+j);
                        const V b1=O::load(right+(k+1)*Tile+j);
                        const V b2=O::load(right+(k+2)*Tile+j);
                        const V b3=O::load(right+(k+3)*Tile+j);
                        if constexpr(Rows>0) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a00,b0,inf,upper); else v0=O::add(a00,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a01,b1,inf,upper); else v1=O::add(a01,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a02,b2,inf,upper); else v2=O::add(a02,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a03,b3,inf,upper); else v3=O::add(a03,b3);
                            O::update(dst+(i+0)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>1) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a10,b0,inf,upper); else v0=O::add(a10,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a11,b1,inf,upper); else v1=O::add(a11,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a12,b2,inf,upper); else v2=O::add(a12,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a13,b3,inf,upper); else v3=O::add(a13,b3);
                            O::update(dst+(i+1)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>2) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a20,b0,inf,upper); else v0=O::add(a20,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a21,b1,inf,upper); else v1=O::add(a21,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a22,b2,inf,upper); else v2=O::add(a22,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a23,b3,inf,upper); else v3=O::add(a23,b3);
                            O::update(dst+(i+2)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                        if constexpr(Rows>3) {
                            V v0; if constexpr(Checked) v0=O::sum_checked(a30,b0,inf,upper); else v0=O::add(a30,b0);
                            V v1; if constexpr(Checked) v1=O::sum_checked(a31,b1,inf,upper); else v1=O::add(a31,b1);
                            V v2; if constexpr(Checked) v2=O::sum_checked(a32,b2,inf,upper); else v2=O::add(a32,b2);
                            V v3; if constexpr(Checked) v3=O::sum_checked(a33,b3,inf,upper); else v3=O::add(a33,b3);
                            O::update(dst+(i+3)*Tile+j,O::maximum(O::maximum(v0,v1),O::maximum(v2,v3)));
                        }
                    }
                }
            }
            return;
        }

        else if constexpr(Disjoint && Columns>0) {
            for(std::size_t i=0;i<Tile;i+=Rows) for(std::size_t j=0;j<Tile;j+=Columns*O::width) {
                V acc[Rows][Columns];
                for(int r=0;r<Rows;++r) for(int c=0;c<Columns;++c)
                    acc[r][c]=O::load(dst+(i+r)*Tile+j+c*O::width);
                for(std::size_t k=0;k<Tile;++k) {
                    V rhs[Columns];
                    for(int c=0;c<Columns;++c) rhs[c]=O::load(right+k*Tile+j+c*O::width);
                    for(int r=0;r<Rows;++r) {
                        const V a=O::broadcast(left[(i+r)*Tile+k]);
                        for(int c=0;c<Columns;++c) {
                            V candidate;
                            if constexpr(Checked) candidate=O::sum_checked(a,rhs[c],inf,upper);
                            else candidate=O::add(a,rhs[c]);
                            acc[r][c]=O::maximum(acc[r][c],candidate);
                        }
                    }
                }
                for(int r=0;r<Rows;++r) for(int c=0;c<Columns;++c)
                    O::store(dst+(i+r)*Tile+j+c*O::width,acc[r][c]);
            }
            return;
        } else {
        for(std::size_t k=0;k<Tile;k+=KU) for(std::size_t i=0;i<Tile;i+=Rows) {
            V coef[Rows][KU];
            for(int r=0;r<Rows;++r) for(int t=0;t<KU;++t)
                coef[r][t]=O::broadcast(left[(i+r)*Tile+k+t]);
            for(std::size_t j=0;j<Tile;j+=O::width) {
                V rhs[KU];
                for(int t=0;t<KU;++t) rhs[t]=O::load(right+(k+t)*Tile+j);
                for(int r=0;r<Rows;++r) {
                    V candidate;
                    if constexpr(Checked) candidate=O::sum_checked(coef[r][0],rhs[0],inf,upper);
                    else candidate=O::add(coef[r][0],rhs[0]);
                    for(int t=1;t<KU;++t) {
                        V next;
                        if constexpr(Checked) next=O::sum_checked(coef[r][t],rhs[t],inf,upper);
                        else next=O::add(coef[r][t],rhs[t]);
                        candidate=O::maximum(candidate,next);
                    }
                    O::update(dst+(i+r)*Tile+j,candidate);
                }
            }
        }
            }
    }

    template<bool Checked> SF_TARGET("avx512f") void recursive512(T** data,std::size_t blocks,
            std::size_t i,std::size_t k,std::size_t j,std::size_t span) {
        if(i>=blocks || k>=blocks || j>=blocks) return;
        if(span==1) {
            T* dst=data[i*blocks+j];
            const T* left=data[i*blocks+k];
            const T* right=data[k*blocks+j];
            if(i!=k && j!=k) tile512<Checked,true>(dst,left,right);
            else tile512<Checked,false>(dst,left,right);
            return;
        }
        const auto half=span/2;
        // Visit the pivot quadrant first, then its row/column, then the remainder.
        // Completing both pivot halves preserves Floyd's dependency order.
        for(std::size_t pk=0;pk<2;++pk) for(std::size_t pi=0;pi<2;++pi)
            for(std::size_t pj=0;pj<2;++pj)
                recursive512<Checked>(data,blocks,i+((pk+pi)%2)*half,k+pk*half,
                    j+((pk+pj)%2)*half,half);
    }
    template<bool Checked> SF_TARGET("avx512f") void schedule512(T** data,std::size_t blocks) {
        if constexpr(Recursive) {
            std::size_t span=1; while(span<blocks) span*=2;
            recursive512<Checked>(data,blocks,0,0,0,span);
            return;
        } else {
        auto tile=[&](std::size_t i,std::size_t j) { return data[i*blocks+j]; };
        for(std::size_t k=0;k<blocks;++k) {
            T* diagonal=tile(k,k);
            tile512<Checked,false>(diagonal,diagonal,diagonal);
            for(std::size_t j=0;j<blocks;++j) if(j!=k)
                tile512<Checked,false>(tile(k,j),diagonal,tile(k,j));
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                tile512<Checked,false>(tile(i,k),tile(i,k),diagonal);
            for(std::size_t i=0;i<blocks;++i) if(i!=k)
                for(std::size_t j=0;j<blocks;++j) if(j!=k)
                    tile512<Checked,true>(tile(i,j),tile(i,k),tile(k,j));
        }
            }
    }
#endif
};
} // namespace simd_floyd
#undef SF_TARGET
#undef SF_INLINE
#undef SF_X86



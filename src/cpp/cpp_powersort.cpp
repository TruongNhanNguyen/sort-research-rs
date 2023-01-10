#include "thirdparty/powersort/powersort.h"
#include "thirdparty/powersort/powersort_4way.h"

#include <compare>
#include <stdexcept>
#include <vector>

#include <stdint.h>

#include "shared.h"

template <typename T>
using vec_iter = std::vector<T>::iterator;

template <typename T>
using powersort = algorithms::powersort<
    /*Iterator=*/T,
    /*minRunLen=*/24,
    /*mergingMethod*/ algorithms::merging_methods::COPY_BOTH,
    /*onlyIncreasingRuns=*/false,
    /*nodePowerImplementation=*/algorithms::MOST_SIGNIFICANT_SET_BIT,
    /*usePowerIndexedStack=*/false>;

template <typename T>
using powersort_4way = algorithms::powersort_4way<
    /*Iterator=*/T,
    /*minRunLen=*/24,
    // For faster perf use WILLEM_TUNED but this can't sort slices with custom
    // types anymore, and it can't correctly sort slices that contain the
    // sentinel. GENERAL_BY_STAGES works without sentinel requirement.
    /*mergingMethod*/ algorithms::merging4way_methods::GENERAL_BY_STAGES,
    /*onlyIncreasingRuns=*/false,
    /*nodePowerImplementation=*/algorithms::MOST_SIGNIFICANT_SET_BIT4,
    /*useParallelArraysForStack=*/false,
    /*useCheckFirstMergeLoop=*/true,
    /*useSpecialized3wayMerge=*/true>;

template <typename T, template <typename> class SortT>
uint32_t sort_by_impl(T* data,
                      size_t len,
                      CompResult (*cmp_fn)(const T&, const T&, uint8_t*),
                      uint8_t* ctx) noexcept {
  try {
    // Powersort does not provide a way to specify a custom comparator function,
    // so we have to wrap it inside a type with custom comparison function.
    CompWrapper<T>::cmp_fn_local = cmp_fn;
    CompWrapper<T>::ctx_local = ctx;

    using iter_t = vec_iter<CompWrapper<T>>;
    // Let's just pray they are layout equivalent.
    SortT<iter_t>{}.sort(iter_t{reinterpret_cast<CompWrapper<T>*>(data)},
                         iter_t{reinterpret_cast<CompWrapper<T>*>(data + len)});
  } catch (const std::exception& exc) {
    // fprintf(stderr, "[ERROR]: %s\n", exc.what());
    return 1;
  } catch (...) {
    return 1;
  }

  return 0;
}

extern "C" {
// --- i32 ---

void powersort_stable_i32(int32_t* data, size_t len) {
  // Uses default configuration.
  using iter_t = vec_iter<int32_t>;
  powersort<iter_t>{}.sort(iter_t{data}, iter_t{data + len});
}

uint32_t powersort_stable_i32_by(int32_t* data,
                                 size_t len,
                                 CompResult (*cmp_fn)(const int32_t&,
                                                      const int32_t&,
                                                      uint8_t*),
                                 uint8_t* ctx) {
  return sort_by_impl<int32_t, powersort>(data, len, cmp_fn, ctx);
}

// --- u64 ---

void powersort_stable_u64(uint64_t* data, size_t len) {
  // Uses default configuration.
  using iter_t = vec_iter<uint64_t>;
  powersort<iter_t>{}.sort(iter_t{data}, iter_t{data + len});
}

uint32_t powersort_stable_u64_by(uint64_t* data,
                                 size_t len,
                                 CompResult (*cmp_fn)(const uint64_t&,
                                                      const uint64_t&,
                                                      uint8_t*),
                                 uint8_t* ctx) {
  return sort_by_impl<uint64_t, powersort>(data, len, cmp_fn, ctx);
}

// --- ffi_string ---

void powersort_stable_ffi_string(FFIString* data, size_t len) {
  using iter_t = vec_iter<FFIStringCpp>;
  powersort<iter_t>{}.sort(iter_t{reinterpret_cast<FFIStringCpp*>(data)},
                           iter_t{reinterpret_cast<FFIStringCpp*>(data) + len});
}

uint32_t powersort_stable_ffi_string_by(FFIString* data,
                                        size_t len,
                                        CompResult (*cmp_fn)(const FFIString&,
                                                             const FFIString&,
                                                             uint8_t*),
                                        uint8_t* ctx) {
  return sort_by_impl<FFIString, powersort>(data, len, cmp_fn, ctx);
}

// --- f128 ---

void powersort_stable_f128(F128* data, size_t len) {
  using iter_t = vec_iter<F128Cpp>;
  powersort<iter_t>{}.sort(iter_t{reinterpret_cast<F128Cpp*>(data)},
                           iter_t{reinterpret_cast<F128Cpp*>(data) + len});
}

uint32_t powersort_stable_f128_by(F128* data,
                                  size_t len,
                                  CompResult (*cmp_fn)(const F128&,
                                                       const F128&,
                                                       uint8_t*),
                                  uint8_t* ctx) {
  return sort_by_impl<F128, powersort>(data, len, cmp_fn, ctx);
}

// --- 1k ---

void powersort_stable_1k(FFIOneKiloByte* data, size_t len) {
  using iter_t = vec_iter<FFIOneKiloByteCpp>;
  powersort<iter_t>{}.sort(
      iter_t{reinterpret_cast<FFIOneKiloByteCpp*>(data)},
      iter_t{reinterpret_cast<FFIOneKiloByteCpp*>(data) + len});
}

uint32_t powersort_stable_1k_by(FFIOneKiloByte* data,
                                size_t len,
                                CompResult (*cmp_fn)(const FFIOneKiloByte&,
                                                     const FFIOneKiloByte&,
                                                     uint8_t*),
                                uint8_t* ctx) {
  return sort_by_impl<FFIOneKiloByte, powersort>(data, len, cmp_fn, ctx);
}

// --- 4 way merging ---

// --- i32 ---

void powersort_4way_stable_i32(int32_t* data, size_t len) {
  // Uses default configuration.
  using iter_t = vec_iter<int32_t>;
  powersort_4way<iter_t>{}.sort(iter_t{data}, iter_t{data + len});
}

uint32_t powersort_4way_stable_i32_by(int32_t* data,
                                      size_t len,
                                      CompResult (*cmp_fn)(const int32_t&,
                                                           const int32_t&,
                                                           uint8_t*),
                                      uint8_t* ctx) {
  return sort_by_impl<int32_t, powersort_4way>(data, len, cmp_fn, ctx);
}

// --- u64 ---

void powersort_4way_stable_u64(uint64_t* data, size_t len) {
  // Uses default configuration.
  using iter_t = vec_iter<uint64_t>;
  powersort_4way<iter_t>{}.sort(iter_t{data}, iter_t{data + len});
}

uint32_t powersort_4way_stable_u64_by(uint64_t* data,
                                      size_t len,
                                      CompResult (*cmp_fn)(const uint64_t&,
                                                           const uint64_t&,
                                                           uint8_t*),
                                      uint8_t* ctx) {
  return sort_by_impl<uint64_t, powersort_4way>(data, len, cmp_fn, ctx);
}

// --- ffi_string ---

void powersort_4way_stable_ffi_string(FFIString* data, size_t len) {
  using iter_t = vec_iter<FFIStringCpp>;
  powersort_4way<iter_t>{}.sort(
      iter_t{reinterpret_cast<FFIStringCpp*>(data)},
      iter_t{reinterpret_cast<FFIStringCpp*>(data) + len});
}

uint32_t powersort_4way_stable_ffi_string_by(
    FFIString* data,
    size_t len,
    CompResult (*cmp_fn)(const FFIString&, const FFIString&, uint8_t*),
    uint8_t* ctx) {
  return sort_by_impl<FFIString, powersort_4way>(data, len, cmp_fn, ctx);
}

// --- f128 ---

void powersort_4way_stable_f128(F128* data, size_t len) {
  using iter_t = vec_iter<F128Cpp>;
  powersort_4way<iter_t>{}.sort(iter_t{reinterpret_cast<F128Cpp*>(data)},
                                iter_t{reinterpret_cast<F128Cpp*>(data) + len});
}

uint32_t powersort_4way_stable_f128_by(F128* data,
                                       size_t len,
                                       CompResult (*cmp_fn)(const F128&,
                                                            const F128&,
                                                            uint8_t*),
                                       uint8_t* ctx) {
  return sort_by_impl<F128, powersort_4way>(data, len, cmp_fn, ctx);
}

// --- 1k ---

void powersort_4way_stable_1k(FFIOneKiloByte* data, size_t len) {
  using iter_t = vec_iter<FFIOneKiloByteCpp>;
  powersort_4way<iter_t>{}.sort(
      iter_t{reinterpret_cast<FFIOneKiloByteCpp*>(data)},
      iter_t{reinterpret_cast<FFIOneKiloByteCpp*>(data) + len});
}

uint32_t powersort_4way_stable_1k_by(FFIOneKiloByte* data,
                                     size_t len,
                                     CompResult (*cmp_fn)(const FFIOneKiloByte&,
                                                          const FFIOneKiloByte&,
                                                          uint8_t*),
                                     uint8_t* ctx) {
  return sort_by_impl<FFIOneKiloByte, powersort_4way>(data, len, cmp_fn, ctx);
}
}  // extern "C"

#pragma once

/**
 * @file OpenMPCompat.h
 * @brief Small OpenMP compatibility wrapper.
 *
 * This header provides a tiny API to query OpenMP state (enabled/threads/id)
 * without scattering preprocessor checks across the codebase.
 *
 * - If compiled with OpenMP support, it includes <omp.h> and forwards to the runtime.
 * - If not, it provides serial fallbacks (1 thread, thread id 0).
 *
 * Usage:
 *   if (ompcompat::enabled()) { ... }
 *   int n = ompcompat::maxThreads();
 *   int tid = ompcompat::threadId();
 */
#ifdef _OPENMP
#include <omp.h>
#endif

namespace OpenMPCompat {

/**
 * @brief Returns true if this translation unit was compiled with OpenMP enabled.
 *
 * This checks the compiler-defined macro `_OPENMP` which is set when the
 * OpenMP compile flags are active (e.g. via linking `OpenMP::OpenMP_CXX` in CMake).
 */
inline constexpr bool enabled() noexcept {
#ifdef _OPENMP
  return true;
#else
  return false;
#endif
}

/**
 * @brief Returns the calling thread's OpenMP thread id inside a parallel region.
 *
 * @return Thread id in [0, numThreads-1]. Returns 0 when OpenMP is disabled.
 */
inline int threadId() noexcept {
#ifdef _OPENMP
  return omp_get_thread_num();
#else
  return 0;
#endif
}

/**
 * @brief Returns the number of threads in the current OpenMP parallel region.
 *
 * Outside a parallel region, OpenMP typically returns 1.
 * Returns 1 when OpenMP is disabled.
 */
inline int numThreads() noexcept {
#ifdef _OPENMP
  return omp_get_num_threads();
#else
  return 1;
#endif
}

/**
 * @brief Returns the maximum number of OpenMP threads the runtime may use.
 *
 * Note: The actual number of threads in a specific parallel region may differ
 * (e.g. due to runtime limits, `OMP_NUM_THREADS`, Slurm cpus-per-task, etc.).
 *
 * @return Max thread count (>= 1). Returns 1 when OpenMP is disabled.
 */
inline int maxThreads() noexcept {
#ifdef _OPENMP
  return omp_get_max_threads();
#else
  return 1;
#endif
}

/**
 * @brief Returns TRUE if inside a  parallel region.
 *
 * Outside a parallel region, OpenMP typically returns FALSE
 * Returns FALSE when OpenMP is disabled.
 */
inline bool inParallel() noexcept {
#ifdef _OPENMP
  return omp_in_parallel() != 0;
#else
  return false;
#endif
}

}  // namespace OpenMPCompat

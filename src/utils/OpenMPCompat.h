#pragma once

// OpenMPCompat.h
// A wrapper so the rest of the code doesn't use #ifdef _OPENMP.
//
// Behavior:
// - If compiled without OpenMP support, everything falls back to serial defaults.
// - If compiled with OpenMP support (-fopenmp), functions call omp_* functionality.

#ifdef _OPENMP
  #include <omp.h>
#endif

namespace omp_compat {

// True if the binary was compiled with OpenMP enabled.
inline constexpr bool enabled() noexcept {
#ifdef _OPENMP
  return true;
#else
  return false;
#endif
}

// Thread id in the *current* parallel region. Serial fallback: 0.
inline int threadId() noexcept {
#ifdef _OPENMP
  return omp_get_thread_num();
#else
  return 0;
#endif
}

// Number of threads in the *current* parallel region. Serial fallback: 1.
inline int numThreads() noexcept {
#ifdef _OPENMP
  return omp_get_num_threads();
#else
  return 1;
#endif
}

// Max threads available to OpenMP runtime (depends on OMP_NUM_THREADS this needs to be set exported in slurm scripts explictely)
//. Serial fallback: 1.
inline int maxThreads() noexcept {
#ifdef _OPENMP
  return omp_get_max_threads();
#else
  return 1;
#endif
}

// True if called from inside a parallel region. Serial fallback: false.
inline bool inParallel() noexcept {
#ifdef _OPENMP
  return omp_in_parallel() != 0;
#else
  return false;
#endif
}

}  // namespace omp_compat

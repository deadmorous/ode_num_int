// timing_util_opaque.cpp

#ifdef _WIN32
#include <intrin.h>
#endif // _WIN32

#ifdef __linux__
#include <x86intrin.h>
#endif // __linux__

#include "timing_util_opaque.h"
#include "ctmstd/cxx_assert.h"

void OpaqueTickCounter::Start()
    {
    ASSERT( tStart == ~0 );
    tStart = __rdtsc();
    }

OpaqueTickType OpaqueTickCounter::Stop()
    {
    ASSERT( tStart != ~0 );
    OpaqueTickType tStop = __rdtsc();
    OpaqueTickType dt = TimeSpan( tStart, tStop );
    tStart = ~0;
    return dt;
    }

OpaqueTickType OpaqueTickCounter::Lap()
    {
    ASSERT( tStart != ~0 );
    OpaqueTickType tStartPrev = tStart;
    tStart = __rdtsc();
    return TimeSpan( tStartPrev, tStart );
    }

// OpaqueTickCounter.h

#ifndef _TIMING_OPAQUETICKCOUNTER_H_CF3E_424f_9766_BA04D388199F_
#define _TIMING_OPAQUETICKCOUNTER_H_CF3E_424f_9766_BA04D388199F_

#include "../defs.h"

#ifdef _WIN32
typedef unsigned __int64 OpaqueTickType;
#endif // _WIN32

#ifdef __linux__
typedef unsigned long long OpaqueTickType;
#endif // __linux__

class ODE_NUMINT_API OpaqueTickCounter
    {
    private:
        OpaqueTickType tStart;
        static OpaqueTickType TimeSpan( const OpaqueTickType& t1, const OpaqueTickType& t2 ) {
            return t2 - t1;
            }
    public:
        // Constructor that optionally starts the stopwatch
        explicit OpaqueTickCounter( bool bStart = false ) : tStart( ~0 ) {
            if( bStart )
                Start();
            }
        // Starts the stopwatch
        void Start();

        // Stops the stopwatch and returns the number of tick since last call to Start() or Lap().
        OpaqueTickType Stop();

        // Returns the number of tick since last call to Start() or Lap(). Records
        // the current tick value as the starting value.
        OpaqueTickType Lap();
    };

#endif // _TIMING_OPAQUETICKCOUNTER_H_CF3E_424f_9766_BA04D388199F_

// TimingStats.h

#ifndef _TIMING_TIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _TIMING_TIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#define CTM_ODE_NUM_INT_ENABLE_TIMING

#ifdef CTM_ODE_NUM_INT_ENABLE_TIMING
#include "OpaqueTickCounter.h"

namespace ctm {
namespace sys {

class TimingStats
    {
    public:
        TimingStats() : m_time(0), m_count(0) {}
        TimingStats& operator+=( const TimingStats& that ) {
            m_count += that.m_count;
            m_time += that.m_time;
            return *this;
            }
        TimingStats operator+( const TimingStats& that ) const {
            TimingStats result = *this;
            result += that;
            return result;
            }

        OpaqueTickType time() const {
            return m_time;
            }
        unsigned int count() const {
            return m_count;
            }
        OpaqueTickType averageTime() const {
            return m_count > 0? m_time/m_count: 0;
            }
        void add( OpaqueTickType t ) {
            m_time += t;
            ++m_count;
            }

    private:
        OpaqueTickType m_time;
        unsigned int m_count;
    };

class ScopedTimeMeasurer
    {
    public:
        explicit ScopedTimeMeasurer( TimingStats& st ) :
            m_st( st ),
            m_timer( true )
            {}

        ~ScopedTimeMeasurer() {
            m_st.add( m_timer.Stop() );
            }

    private:
        TimingStats& m_st;
        OpaqueTickCounter m_timer;
    };

} // end namespace sys
} // end namespace ctm

#else // CTM_ODE_NUM_INT_ENABLE_TIMING

namespace ctm {
namespace sys {

class TimingStats
    {
    public:
        unsigned int time() const { return 0; }
        unsigned int count() const { return 0; }
        unsigned int averageTime() const { return 0; }
        void add( unsigned int ) {}

        TimingStats& operator+=( const TimingStats& that ) {
            return *this;
            }
        TimingStats operator+( const TimingStats& that ) const {
            return TimingStats();
            }
    };

class ScopedTimeMeasurer
    {
    public:
        explicit ScopedTimeMeasurer( TimingStats& ) {}
    };

} // end namespace sys
} // end namespace ctm

#endif // CTM_ODE_NUM_INT_ENABLE_TIMING

#endif // _TIMING_TIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// timer_calibrator.cpp

#include "timing/TimerCalibrator.h"
#include "infra/cxx_assert.h"
#include "util/sys_time.h"
#include "infra/cxx_exception.h"
#include <sstream>

namespace ctm {
namespace sys {

TimerCalibrator::TimerCalibrator() :
    m_timer( true ),
    m_timerInterval( 0 ),
    m_msecInterval( sys::GetClockTimeMsec() ),
    m_stopped( false )
    {}

void TimerCalibrator::stop()
    {
    ASSERT( !m_stopped );
    m_timerInterval = m_timer.Stop();
    ASSERT( m_timerInterval > 0 );
    m_msecInterval = sys::GetClockTimeMsec() - m_msecInterval;
    m_stopped = true;
    }

bool TimerCalibrator::toMsecAvailable() const {
    return m_stopped   &&   m_msecInterval >= MsecIntervalThreshold   &&   m_timerInterval > 0;
    }

bool TimerCalibrator::stopped() const {
    return m_stopped;
    }

double TimerCalibrator::toMsec( OpaqueTickType interval ) const
    {
    ASSERT( m_stopped );
    if( m_msecInterval < MsecIntervalThreshold )
        throw cxx::exception( "TimerCalibrator::toMsec() failed: measurement interval is too short" );
    return static_cast<double>(interval) * m_msecInterval / m_timerInterval;
    }

std::string TimerCalibrator::formatTimeInterval( OpaqueTickType interval, bool noTicksIfPossible ) const
    {
    ASSERT( m_stopped );
    std::ostringstream oss;
    if( noTicksIfPossible && toMsecAvailable() )
        oss << toMsec( interval ) << " msec";
    else {
        oss << interval << " ticks";
        if( toMsecAvailable() )
            oss << " = " << toMsec( interval ) << " msec";
        }
    return oss.str();
    }

} // end namespace sys
} // end namespace ctm

// timer_calibrator.h

#ifndef _TIMER_CALIBRATOR_H_
#define _TIMER_CALIBRATOR_H_

#include "timing_util_opaque.h"
#include <string>

namespace ctm {
namespace sys {

class TimerCalibrator
    {
    public:
        TimerCalibrator();
        void stop();
        bool toMsecAvailable() const;
        bool stopped() const;
        double toMsec( OpaqueTickType interval ) const;
        std::string formatTimeInterval( OpaqueTickType interval, bool noTicksIfPossible = true ) const;
    private:
        static const unsigned long MsecIntervalThreshold = 100;
        OpaqueTickCounter m_timer;
        OpaqueTickType m_timerInterval;
        unsigned long m_msecInterval;
        bool m_stopped;
    };

} // end namespace sys
} // end namespace ctm

#endif // _TIMER_CALIBRATOR_H_

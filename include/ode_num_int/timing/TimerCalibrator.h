// TimerCalibrator.h

#ifndef _TIMING_TIMERCALIBRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _TIMING_TIMERCALIBRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OpaqueTickCounter.h"
#include <string>

namespace ctm {
namespace sys {

class ODE_NUMINT_API TimerCalibrator
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

#endif // _TIMING_TIMERCALIBRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

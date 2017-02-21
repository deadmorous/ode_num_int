// sys_time.h

/** \ingroup CTMSTD_SYS*/
/** \defgroup CTMSTD_SYS_TIME Time functions
This module defines some time-related functions that have no analogs
in the C run-time library and thus are platform dependent. Use definitions
from this file instead of OS-dependent API to make your programs more portable.

<b> Supported platforms:</b> Linux, Win32.

<b> Module type:</b> header file.

<b> Linkage instructions</b>

On Linux, link program to the librt library (specify the \c -lrt link option)
*/

/// \ingroup CTMSTD_SYS_TIME
/// \file sys_time.h
/// \brief \ref CTMSTD_SYS_TIME

#ifndef _SYS_TIME_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_
#define _SYS_TIME_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

#include <sstream>
#include <iomanip>

#ifdef _WIN32               // Platform
#ifndef _WINBASE_

extern "C" __declspec(dllimport) unsigned long __stdcall GetTickCount();
extern "C" __declspec(dllimport) void __stdcall Sleep( unsigned long dwMilliseconds );

#endif // !_WINBASE_

#elif defined __linux       // Platform
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>

#else                       // Platform
#error "Unknown platform"

#endif                      // Platform

namespace ctm {
namespace sys {

#ifdef _WIN32               // Platform

/** \addtogroup CTMSTD_SYS_TIME */
//@{

/// \brief Returns time, in milliseconds, elapsed since system startup.
inline unsigned long GetClockTimeMsec() {
    return ::GetTickCount();
    }

//@}
#elif defined __linux       // Platform

inline unsigned long GetClockTimeMsec() {
    timespec ts;
    VERIFY( clock_gettime( CLOCK_MONOTONIC, &ts ) == 0 );
    unsigned long msec = ts.tv_sec*1000 + ts.tv_nsec/1000000;
    return msec;
    }

#else                       // Platform
#error "Unknown platform"

#endif                      // Platform

inline std::string FormatTimeSpanSec( unsigned long sec )
    {
    unsigned int timeHr = sec / 3600;
    unsigned int timeMin = ( sec / 60 ) % 60;
    unsigned int timeSec = sec - 60 * ( 60*timeHr + timeMin );
    std::ostringstream oss;
    oss << timeHr << ":"
        << std::setw(2) << std::setfill('0') << timeMin << ":"
        << std::setw(2) << std::setfill('0') << timeSec;
    return oss.str();
    }

inline std::string FormatTimeSpanMsec( unsigned long msec )
    {
    std::ostringstream oss;
    oss << '.' << std::setw(3) << std::setfill('0') << msec%1000;
    return FormatTimeSpanSec(msec/1000) + oss.str();
    }

#ifdef _WIN32               // Platform
inline void SleepSeconds( int sec ) {
    Sleep( sec*1000 );     // BUGgy: overflow
    }
#elif defined __linux       // Platform
inline void SleepSeconds( int sec ) {
    sleep( sec );
    }
#endif                      // Platform

} // end namespace sys
} // end namespace ctm

#endif // _SYS_TIME_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

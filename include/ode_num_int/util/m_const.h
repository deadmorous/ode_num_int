// m_const.h

/** \ingroup CTMSTD*/
/** \defgroup CTMSTD_MATH Mathematics*/

/** \ingroup CTMSTD_MATH*/
/** \defgroup CTMSTD_MATH_CONST Declarations for mathematical constants
Contains declarations for some mathematical constants.

<b> Supported platforms:</b> GNU/Linux, Win32.

<b> Module type:</b> header file.
*/

/// \ingroup CTMSTD_MATH_CONST
/// \file m_const.h
/// \brief \ref CTMSTD_MATH_CONST

#ifndef _UTIL_M_CONST_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_M_CONST_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#ifdef __gnu_linux__
#include <math.h>
#else // __gnu_linux__

/** \addtogroup CTMSTD_MATH_CONST*/
//@{

#ifndef M_PI
#define M_PI            3.14159265358979323846  ///< The 'Pi' constant
#endif // M_PI

#ifndef M_PI_2
#define M_PI_2          (M_PI/2)                ///< A half of the 'Pi' constant
#endif // M_PI_2

//@}

#endif // __gnu_linux__

#endif // _UTIL_M_CONST_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

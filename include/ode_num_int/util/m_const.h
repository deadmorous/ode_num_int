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

#ifndef _EX_PI_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_
#define _EX_PI_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

#ifdef __gnu_linux__
#include <math.h>
#else // __gnu_linux__

/** \addtogroup CTMSTD_MATH_CONST*/
//@{

#define M_PI            3.14159265358979323846  ///< The 'Pi' constant
#define M_PI_2          (M_PI/2)                ///< A half of the 'Pi' constant

//@}

#endif // __gnu_linux__

#endif // _EX_PI_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

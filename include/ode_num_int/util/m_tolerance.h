// m_tolerance.h

/** \ingroup CTMSTD_MATH*/
/** \defgroup CTMSTD_MATH_TOLERANCE Constant values for typical tolerances.
Defines several frequently used tolerance values.
\note The constants are declared as macros, which may cause name conflicts,
as long as the macros cannot be placed into a namespace.

<b> Supported platforms:</b> platform independent.

<b> Module type:</b> header file.
*/

/// \ingroup CTMSTD_MATH_TOLERANCE
/// \file m_tolerance.h
/// \brief \ref CTMSTD_MATH_TOLERANCE

#ifndef _UTIL_M_TOLERANCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_M_TOLERANCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

/** \addtogroup CTMSTD_MATH_TOLERANCE*/
//@{

/// \brief Relative tolerance for geometry precision.
#define GEOM_EPS    1e-5

/// \brief Relative tolerance for machine number precision.
#define NUMBER_EPS  1e-8

//@}

#endif // _UTIL_M_TOLERANCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

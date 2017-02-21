// m_inline.h

/** \ingroup CTMSTD_MATH*/
/** \defgroup CTMSTD_MATH_INLINE Simple inline math functions.
Defines several frequently used mathematic functions.

<b> Supported platforms:</b> platform independent.

<b> Module type:</b> header file.
*/

/// \ingroup CTMSTD_MATH_INLINE
/// \file m_inline.h
/// \brief \ref CTMSTD_MATH_INLINE

#ifndef _M_INLINE_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_
#define _M_INLINE_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

#include "cxx_assert.h"
#include <math.h>

namespace ctm {
namespace math {

/** \addtogroup CTMSTD_MATH_INLINE*/
//@{

/// \brief Returns the square of specified value.
template< class X >
inline double sqr( X x ) { return x*x; }

/// \brief Returns the sign of specified value.
/// \param x the value whose sign is to be calculated.
/// \return -1 if \a x is less than zero; +1 otherwise.
inline double Sgn( double x ) { return x < 0 ?   -1 :   1; }

/// \brief Returns minimum of two specified values.
template< class X >
inline X Min( X a, X b ) { return a < b ?   a :   b; }

/// \brief Returns minimum of two specified values.
template< class X >
inline X Max( X a, X b ) { return a > b ?   a :   b; }

// Returns nearest integer to specified x.
inline int roundi( double x ) {
    return x >= 0 ?
        static_cast<int>( x + 0.5 ) :
       -static_cast<int>( -x + 0.5 );
    }

// Returns remainder, taking the sign of x into account
inline unsigned int modp( int x, unsigned int m ) {
    ASSERT( m );
    unsigned int xx;
    if( x < 0 ) {
        xx = m - (-x)%m;
        if( xx == m )
            xx = 0;
        }
    else
        xx = x % m;
    return xx;
    }

// Returns "remainder" in range [bias, bias+period)
inline double modd( double x, double period, double bias )
    {
    x -= bias;
    x -= floor( x/period )*period;
    x += bias;
    return x;
    }

namespace cubic {
    template< class X > inline X f00( X t ) { return 1 + t*t*(t*2 - 3); }
    template< class X > inline X f01( X t ) { return t*t*(3 - t*2); }
    template< class X > inline X f10( X t ) { return t*( 1 + t*( t - 2 ) ); }
    template< class X > inline X f11( X t ) { return t*t*( t - 1 ); }

    template< class X > inline X df00( X t ) { return t*(t*6 - 6); }
    template< class X > inline X df01( X t ) { return t*(6 - t*6); }
    template< class X > inline X df10( X t ) { return 1 + t*( 3*t - 4 ); }
    template< class X > inline X df11( X t ) { return t*( 3*t - 2 ); }

    template< class X > inline X d2f00( X t ) { return t*12 - 6; }
    template< class X > inline X d2f01( X t ) { return 6 - t*12; }
    template< class X > inline X d2f10( X t ) { return 6*t - 4; }
    template< class X > inline X d2f11( X t ) { return 6*t - 2; }

    template< class X > inline X f00( X x, X L ) { return f00( x/L ); }
    template< class X > inline X f01( X x, X L ) { return f01( x/L ); }
    template< class X > inline X f10( X x, X L ) { return f10( x/L ) * L; }
    template< class X > inline X f11( X x, X L ) { return f11( x/L ) * L; }

    template< class X > inline X df00( X x, X L ) { return df00( x/L ) / L; }
    template< class X > inline X df01( X x, X L ) { return df01( x/L ) / L; }
    template< class X > inline X df10( X x, X L ) { return df10( x/L ); }
    template< class X > inline X df11( X x, X L ) { return df11( x/L ); }

    template< class X > inline X d2f00( X x, X L ) { return d2f00( x/L ) / (L*L); }
    template< class X > inline X d2f01( X x, X L ) { return d2f01( x/L ) / (L*L); }
    template< class X > inline X d2f10( X x, X L ) { return d2f10( x/L ) / L; }
    template< class X > inline X d2f11( X x, X L ) { return d2f11( x/L ) / L; }

    // <RU> Funkcii dlja vychislenija kubicheskix polinomov s zadannymi znachenijami funkcii i ee proizvodnoj </RU>
    // <RU> na koncax otrezka [0,1] </RU>

    inline double CalcCubicPolynom( double f0, double df0, double f1, double df1, double t )
        { return f0 + t * ( df0 + t * ( 3*(f1-f0)-2*df0-df1 + t * ( df1+df0+2*(f0-f1) ) ) ); }

    inline double CalcCubicPolynomD( double f0, double df0, double f1, double df1, double t )
        { return df0 + t * ( 6*(f1-f0)-4*df0-2*df1 + t * ( 3*(df1+df0)+6*(f0-f1) ) ); }

    inline double CalcCubicPolynomD2( double f0, double df0, double f1, double df1, double t )
        { return 6*(f1-f0)-4*df0-2*df1 + t * ( 6*(df1+df0)+12*(f0-f1) ); }

    inline double CalcCubicPolynomD3( double f0, double df0, double f1, double df1, double t )
        { return 6*(df1+df0)+12*(f0-f1); }

} // end namespace cubic

//@}

} // end namespace math
} // end namespace ctm

#endif // _M_INLINE_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

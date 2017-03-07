// cxx_zeroinit.h

#ifndef _CXX_ZEROINIT_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_
#define _CXX_ZEROINIT_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

/** \ingroup CTMSTD_CXX*/
/** \defgroup CTMSTD_CXX_ZEROINIT Zero-initialized types
It's often necessary to initialize a class member with zero value. While it is possible
to do so in constructors, there is a better alternative. Taking care of initialization
with zero requires to ensure that all (but copy) constructors really do this job; besides,
sometimes constructors are declared by using a macro, or the values to be zero-initialized
are declared using a macro, and we don't want to make developer bored of all that stuff.

Here we encourage developers to use types that behave exactly like standard types
(\b int, \b long, \b float, etc.), with the only difference that they are zero-initialized
by default. This allows to use these types for those member variables which must be always
initialized with zero.

<b> Supported platforms:</b> platform independent.

<b> Module type:</b> header file.
*/

/// \ingroup CTMSTD_CXX_ZEROINIT
/// \file cxx_zeroinit.h
/// \brief \ref CTMSTD_CXX_ZEROINIT
///
/// This file declares a template class for zero-initialized types, and some typedefs
/// for standard types.

namespace ctm {
namespace cxx {

/** \addtogroup CTMSTD_CXX_ZEROINIT*/
//@{
/// \brief Template class for zero-initialized objects of numerical types.
///
/// This template class declares a constructor with one argument, zero by default, and
/// basic arithmetic operations. The template argument is normally a standard
/// integer or floating point type.
template< class X >
struct zero_init
    {
    X x;    ///< Instance of object of primary type
    zero_init( X x_ = 0 ) : x( x_ ) {}              ///< Copy- and zero-initializing constructor
    X& operator=( X x_ ) { x = x_;   return x; }    ///< Assignment operator
    operator X() const { return x; }                ///< Automatic conversion operator

    /// \name Modifying operators (arithmetic and logical)
    //@{
    X& operator+=( X arg ) { return x += arg; }     ///< Addition
    X& operator-=( X arg ) { return x -= arg; }     ///< Subtraction
    X& operator&=( X arg ) { return x &= arg; }     ///< Bitwise ANDing
    X& operator|=( X arg ) { return x |= arg; }     ///< Bitwise ORing
    X& operator*=( X arg ) { return x *= arg; }     ///< Multiplying
    X& operator/=( X arg ) { return x /= arg; }     ///< Dividing
    X& operator%=( X arg ) { return x %= arg; }     ///< Taking remainder
    //@}

    /// \name Increment and decrement operators
    //@{
    X& operator++() { return ++x; }                 ///< Pre-increment
    X& operator--() { return --x; }                 ///< Pre-decrement
    X operator++(int) { return x++; }               ///< Post-increment
    X operator--(int) { return x--; }               ///< Post-decrement
    //@}
    };

typedef zero_init<bool> bool0;                      ///< Zero-initialized \b bool
typedef zero_init<char> char0;                      ///< Zero-initialized \b char
typedef zero_init<unsigned char> uchar0;            ///< Zero-initialized \b unsigned char
typedef zero_init<int> int0;                        ///< Zero-initialized \b int
typedef zero_init<unsigned int> uint0;              ///< Zero-initialized \b unsigned int
typedef zero_init<long> long0;                      ///< Zero-initialized \b long
typedef zero_init<unsigned long> ulong0;            ///< Zero-initialized \b unsigned long
typedef zero_init<float> float0;                    ///< Zero-initialized \b float
typedef zero_init<double> double0;                  ///< Zero-initialized \b double

/// \brief Template class for zero-initialized pointers.
///
/// This template class declares a constructor with one argument, zero by default, and
/// basic arithmetic operations. The template argument is any type; template instances
/// behave as pointers to objects of that type.
template< class X >
struct zero_init_ptr
    {
    X *p;    ///< Instance of object of primary type
    zero_init_ptr( X *p_ = 0 ) : p( p_ ) {}         ///< Copy- and zero-initializing constructor
    X*& operator=( X *p_ ) { p = p_;   return p; }  ///< Assignment operator
    operator X*() const { return p; }               ///< Automatic conversion operator

    /// \name Modifying arithmetic operators
    //@{
    X*& operator+=( unsigned int arg ) { return p += arg; } ///< Addition
    X*& operator-=( unsigned int arg ) { return p -= arg; } ///< Subtraction
    //@}

    /// \name Increment and decrement operators
    //@{
    X*& operator++() { return ++p; }                ///< Pre-increment
    X*& operator--() { return --p; }                ///< Pre-decrement
    X* operator++(int) { return p++; }              ///< Post-increment
    X* operator--(int) { return p--; }              ///< Post-decrement
    //@}

    /// \name Member access operator
    //@{
    X* operator->() const { return p; }
    //@}
    };

//@}

} // end namespace cxx
} // end namespace ctm

#endif // _CXX_ZEROINIT_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

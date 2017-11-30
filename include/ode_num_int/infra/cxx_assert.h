// cxx_assert.h

/** \defgroup CTMSTD Standard Extensions used in CTM programs*/

/** \ingroup CTMSTD*/
/** \defgroup CTMSTD_CXX Features related to the C++ language*/

/** \ingroup CTMSTD_CXX*/
/** \defgroup CTMSTD_CXX_ASSERT Assertion macros
This module defines two assertion macros: #ASSERT and #VERIFY.
The macros help the programmer find bugs on early stages by exploiting
the concept of self-testing code. The testing with use the #ASSERT and #VERIFY macros
only occurs in debug builds, and is stripped in release builds. This ensures that
the use of the macros brings no additional overhead is into release versions
of the program.

In your code, whenever you need to be sure that a specific condition is held,
use the #ASSERT macro, specifying the condition as an argument.

If you need to evaluate an expression in both debug and release builds,
but test it in debug builds only, use the #VERIFY macro.
The macro is commonly used to verify that a specific function returns non-zero.

<b> Supported platforms:</b> Linux, Win32.

<b> Module type:</b> header file.
*/

//@{
/// \file cxx_assert.h
/// \brief \ref CTMSTD_CXX_ASSERT

/// \def ASSERT
/// \brief In debug builds, if the argument is zero, displays
/// an error message and aborts program; if the argument is non-zero, does nothing.
/// In release builds, does nothing, not even evaluates the argument expression.

/// \def VERIFY
/// \brief In debug builds, if the argument is zero, displays
/// an error message and aborts program; if the argument is non-zero, does nothing.
/// In release builds, just evaluates the argument expression.
//@}

#ifndef _INFRA_CXX_ASSERT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_CXX_ASSERT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#if defined _WIN32 && !defined __MINGW32__
/////////////////////////////////////////////////////////////////////////////
// Defs for Win32 builds

#ifdef CTM_USE_DBGDUMP

#include "dbgdump/dbgdump.h"

#ifdef ASSERT
#undef ASSERT
#endif // ASSERT
#define ASSERT CTM_DBGDUMP_ASSERT

#ifdef VERIFY
#undef VERIFY
#endif // VERIFY
#define VERIFY CTM_DBGDUMP_ASSERT

#ifdef Q_ASSERT
#undef Q_ASSERT
#endif // Q_ASSERT
#define Q_ASSERT CTM_DBGDUMP_ASSERT

#else // CTM_USE_DBGDUMP

#ifndef    _INC_CRTDBG
#include <crtdbg.h>
#endif  // !_INC_CRTDBG

#ifndef __AFX_H__
#define ASSERT _ASSERTE         // This macro is found in the crtdbg.h file
#endif // !__AFX_H__

#ifndef __AFX_H__
#ifdef     _DEBUG
#define VERIFY( condition )   ASSERT( condition )
#else   // _DEBUG
#define VERIFY( condition )   ( condition )
#endif  // _DEBUG
#endif  // !__AFX_H__

#endif // CTM_USE_DBGDUMP

#elif defined __linux || defined __MINGW32__
/////////////////////////////////////////////////////////////////////////////
// Defs for Linux builds

#include <assert.h>

#ifdef     _DEBUG
#define ASSERT assert
#define VERIFY( condition )   ASSERT( condition )
#else   // _DEBUG
#define ASSERT( condition )
#define VERIFY( condition )   ( condition )
#endif  // _DEBUG


#else
/////////////////////////////////////////////////////////////////////////////
// Stop if targeting to an unsupported platform
#error "Unsupported platform"
#endif

#endif // _INFRA_CXX_ASSERT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

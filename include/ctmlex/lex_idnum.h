// lex_idnum.h

/** \ingroup CTMSTD_LANG*/
/** \defgroup CTMSTD_LANG_LEX General lexical analysis routines
This module implements several commonly used functions for lexical analysis.

<b> Supported platforms:</b> Linux, Win32.

<b> Module type:</b> dynamic library.

<b> Linkage instructions</b>

Use early binding to link the module to your C++ program.
    - In Linux builds, pass the \b ctmlex.so shared library to the linker along with your object files.
    - In Win32 builds, pass the \b ctmlex.lib export library to the linker along with your object files.
*/

/// \ingroup CTMSTD_LANG_LEX
/// \file lex_idnum.h
/// \brief \ref CTMSTD_LANG_LEX

#ifndef _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_
#define _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_

namespace ctm {

/** \ingroup CTMSTD_LANG*/
/// \brief \ref CTMSTD_LANG
namespace lang {

#ifdef _WIN32
#ifdef CTM_LANG_CTMLEX_EXPORTS
#define CTM_LANG_CTMLEX_API __declspec(dllexport)
#else
#define CTM_LANG_CTMLEX_API __declspec(dllimport)
#endif
#else // _WIN32
#define CTM_LANG_CTMLEX_API
#endif // _WIN32

/** \addtogroup CTMSTD_LANG_LEX*/
//@{

/// Returns true if \a str is a C/C++ identifier.
bool CTM_LANG_CTMLEX_API IsIdentifier( const char *str );

/// Returns true if \a str is a real number.
bool CTM_LANG_CTMLEX_API IsNumber( const char *str );

/// Returns true if \a str is an integer number.
bool CTM_LANG_CTMLEX_API IsIntegerNumber( const char *str );

/// Returns true if \a str is a hexadecimal integer number.
bool CTM_LANG_CTMLEX_API IsHexIntegerNumber( const char *str );

//@}

} // end namespace lang
} // end namespace ctm

#endif // _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_

// lex_idnum.h

/** \ingroup CTMSTD_LANG*/
/** \defgroup CTMSTD_LANG_LEX General lexical analysis routines
This module implements several commonly used functions for lexical analysis.

<b> Supported platforms:</b> Platform independent.

<b> Module type:</b> header file.
*/

/// \ingroup CTMSTD_LANG_LEX
/// \file lex_idnum.h
/// \brief \ref CTMSTD_LANG_LEX

#ifndef _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_
#define _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_

#include <regex>

namespace ctm {

/** \ingroup CTMSTD_LANG*/
/// \brief \ref CTMSTD_LANG
namespace lang {

/** \addtogroup CTMSTD_LANG_LEX*/
//@{

/// Returns true if \a str is a C/C++ identifier.
inline bool IsIdentifier( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[a-zA-z_][a-zA-z0-9_]*$" ) );
    }

/// Returns true if \a str is a real number.
inline bool IsNumber( const char *str ) {
    // http://www.regexlib.com/RETester.aspx?regexp_id=185
    return std::regex_match(
                str,
                std::regex( "^[+-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?$" ) );
    }

/// Returns true if \a str is an integer number.
inline bool IsIntegerNumber( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[+-]?[0-9]+$" ) );
    }

/// Returns true if \a str is a hexadecimal integer number.
inline bool IsHexIntegerNumber( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[0-9a-fA-F]+$" ) );
    }

//@}

} // end namespace lang
} // end namespace ctm

#endif // _LEX_IDNUM_H_B8BBDA76_D223_4706_8AD7_5B74AE3D0CD1_

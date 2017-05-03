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

#ifndef _UTIL_LEX_IDNUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_LEX_IDNUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// Note: g++ 4.8.2 under mingw reports regex errors, while with other compilers it's all ok.
// Uncomment macro below to disable regex and force all functions to return true, as if all checks pass.
// #define CTM_UTIL_LEX_IDNUM_DISABLE_REGEX

#ifndef CTM_UTIL_LEX_IDNUM_DISABLE_REGEX
#include <regex>
#endif // CTM_UTIL_LEX_IDNUM_DISABLE_REGEX

namespace ctm {

/** \ingroup CTMSTD_LANG*/
/// \brief \ref CTMSTD_LANG
namespace lang {

/** \addtogroup CTMSTD_LANG_LEX*/
//@{

#ifdef CTM_UTIL_LEX_IDNUM_DISABLE_REGEX

/// \brief Always returns true.
inline bool IsIdentifier( const char * ) {
    return true;
    }

/// \brief Always returns true.
inline bool IsNumber( const char * ) {
    return true;
    }

/// \brief Always returns true.
inline bool IsIntegerNumber( const char * ) {
    return true;
    }

/// \brief Always returns true.
inline bool IsHexIntegerNumber( const char * ) {
    return true;
    }

#else // CTM_UTIL_LEX_IDNUM_DISABLE_REGEX


/// \brief Returns true if \a str is a C/C++ identifier.
inline bool IsIdentifier( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[a-zA-z_][a-zA-z0-9_]*$" ) );
    }

/// \brief Returns true if \a str is a real number.
inline bool IsNumber( const char *str ) {
    // http://www.regexlib.com/RETester.aspx?regexp_id=185
    return std::regex_match(
                str,
                std::regex( "^[+-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?$" ) );
    }

/// \brief Returns true if \a str is an integer number.
inline bool IsIntegerNumber( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[+-]?[0-9]+$" ) );
    }

/// \brief Returns true if \a str is a hexadecimal integer number.
inline bool IsHexIntegerNumber( const char *str ) {
    return std::regex_match(
                str,
                std::regex( "^[0-9a-fA-F]+$" ) );
    }

#endif // CTM_UTIL_LEX_IDNUM_DISABLE_REGEX

//@}

} // end namespace lang
} // end namespace ctm

#endif // _UTIL_LEX_IDNUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

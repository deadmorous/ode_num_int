// cxx_str_fmt_num.h

#ifndef _UTIL_CXX_STR_FMT_NUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_CXX_STR_FMT_NUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include <string>

namespace ctm {
namespace cxx {

inline std::string FormatInt( int i ) {
    return std::to_string( i );
    }

/* TODO
inline std::string FormatHex( unsigned int x )
    {
    static const unsigned int MAX_ITOA = 33;
    char buf[MAX_ITOA];
    itoa( x, buf, 16 );
    return buf;
    }
*/

} // end namespace cxx
} // end namespace ctm

#endif // _UTIL_CXX_STR_FMT_NUM_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

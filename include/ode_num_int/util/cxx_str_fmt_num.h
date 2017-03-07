// cxx_str_fmt_num.h

#ifndef _CXX_STR_FMT_NUM_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_
#define _CXX_STR_FMT_NUM_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

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

#endif // _CXX_STR_FMT_NUM_H_8A20E4E5_42D0_4d8a_B50D_4326E9683BB8_

// util.h

#ifndef _LU_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _LU_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "lu/LUFactorizer.h"
#include "sparse_util.h"

namespace ctm {
namespace math {

template< class real_type >
inline sparse::SparseMatrix< real_type > luToMatrix( const LUFactorizer<real_type>& lu )
    {
    auto n = lu.size();
    sparse::SparseMatrix< real_type > result( n, n );
    for( unsigned int r=0; r<n; ++r )
        for( unsigned int c1=lu.firstCol(r), c2=lu.lastCol(r), c=c1; c<=c2; ++c ) {
            auto x = lu.elementAt( r, c );
            if( x != 0 )
                result.at( r, c ) = x;
            }
    return result;
    }

} // end namespace math
} // end namespace ctm

#endif // _LU_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

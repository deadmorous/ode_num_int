// computeJacobian.h

#ifndef _ALGSOLVER_COMPUTE_JACOBIAN_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_COMPUTE_JACOBIAN_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/VectorMapping.h"
#include "../la/sparse_util.h"

namespace ctm {
namespace math {

template< class Mapping, class V >
inline sparse::SparseMatrix< typename V::value_type > computeJacobian( const Mapping& mapping, const V& x0 )
    {
    typedef typename V::value_type real_type;

    // Determine problem size
    auto n = mapping.inputSize();
    ASSERT( x0.size() == n );
    ASSERT( mapping.outputSize() == n );

    // Allocate storage
    V x(n), f0(n), f(n);

    // Compute rhs at x0
    mapping.map( f0, x0 );

    auto makeX = [&]( unsigned int i, real_type dx ) {
        x = x0;
        x[i] += dx;
        };

    // Compute the Jacobian
    real_type dx = 1e-6;
    sparse::SparseMatrix< typename V::value_type > result( n, n );
    for( unsigned int i=0; i<n; ++i ) {
        makeX( i, dx );
        mapping.map( f, x );
        for( unsigned int j=0; j<n; ++j ) {
            auto df = f[j] - f0[j];
            if( df == 0 )
                continue;
            result.at( j, i ) = df / dx;
            }
        }

    return result;
    }

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_COMPUTE_JACOBIAN_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

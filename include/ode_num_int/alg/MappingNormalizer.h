// MappingNormalizer.h

#ifndef _ALG_MAPPINGNORMALIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_MAPPINGNORMALIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./DiagonalMapping.h"

namespace ctm {
namespace math {

template< class VD >
class MappingNormalizer :
    public VectorMapping< VD >,
    public MappingHolder< VD >,
    public DiagonalMappingHolder< VD >
    {
    public:
        typedef VectorTemplate< VD > V;

        unsigned int inputSize() const {
            return this->mapping()->inputSize();
            }

        unsigned int outputSize() const {
            return this->mapping()->outputSize();
            }

        virtual std::vector<unsigned int> inputIds() const {
            return this->mapping()->inputIds();
            }

        virtual std::vector<unsigned int> outputIds() const {
            return this->mapping()->outputIds();
            }

        void map( V& dst, const V& x ) const
            {
            this->vectorMappingPreObservers( x, this );
            auto& dm = *this->diagonalMapping();
            V arg = dm.VectorMapping<VD>::map( x );
            V dstUnscaled = this->mapping()->map( arg );
            dm.map( dst, dstUnscaled );
            this->vectorMappingPostObservers( x, dst, this );
            }

        void normalize( const V& x0 )
            {
            auto n = inputSize();
            V d( n );
            // TODO jacobianRefreshObservers( true );
            auto J = computeJacobian( *this->mapping(), x0 );
            // TODO jacobianRefreshObservers( true );
            for( unsigned int i=0; i<n; ++i ) {
                auto di = J.get( i, i );
                if( di == 0 )
                    throw cxx::exception( "MappingNormalizer::normalize() failed: zero diagonal element in Jacobian" );
                d[i] = 1. / sqrt( fabs( di ) );
                }
            this->setDiagonalMapping( std::make_shared< DiagonalMapping<VD> >( d ) );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_MAPPINGNORMALIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

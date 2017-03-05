// JacobianWidthBasedTrimmer.h

#ifndef _ALGSOLVER_JACOBIANWIDTHBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_JACOBIANWIDTHBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "JacobianTrimmer.h"

namespace ctm {
namespace math {

template< class VD >
class JacobianWidthBasedTrimmer :
    public JacobianTrimmer<VD>,
    public FactoryMixin< JacobianWidthBasedTrimmer<VD>, JacobianTrimmer<VD> >
    {
    public:
        typedef typename JacobianTrimmer<VD>::SparseMatrix SparseMatrix;

        JacobianWidthBasedTrimmer() : m_width(0) {}

        SparseMatrix trimJacobian( const SparseMatrix& J )
            {
            if( m_width == 0 )
                return J;
            SparseMatrix result( J.size() );
            for( const auto& e : J ) {
                int d = e.first.first - e.first.second;
                if( std::abs( d ) < static_cast<int>(m_width) )
                    result.at( e.first ) = e.second;
                }
            return result;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["width"] = m_width;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters& parameters  ) {
            OptionalParameters::maybeLoadParameter( parameters, "width", m_width );
            }

    private:
        unsigned int m_width;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_JACOBIANWIDTHBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

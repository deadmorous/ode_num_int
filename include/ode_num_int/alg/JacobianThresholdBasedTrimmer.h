// JacobianThresholdBasedTrimmer.h

#ifndef _ALG_JACOBIANTHRESHOLDBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_JACOBIANTHRESHOLDBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/JacobianTrimmer.h"

namespace ctm {
namespace math {

template< class VD >
class JacobianThresholdBasedTrimmer :
    public JacobianTrimmer<VD>,
    public FactoryMixin< JacobianThresholdBasedTrimmer<VD>, JacobianTrimmer<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef typename JacobianTrimmer<VD>::SparseMatrix SparseMatrix;

        JacobianThresholdBasedTrimmer() : m_threshold(0) {}

        SparseMatrix trimJacobian( const SparseMatrix& J )
            {
            if( m_threshold == 0 )
                return J;
            SparseMatrix result( J.size() );
            real_type jmax = 0;
            for( const auto& e : J )
                jmax = std::max( jmax, std::fabs( e.second ) );
            if( jmax == 0 )
                return J;
            auto absThreshold = m_threshold * jmax;
            for( const auto& e : J ) {
                if( e.first.first == e.first.second   ||
                    std::fabs( e.second ) >= absThreshold )
                    result.at( e.first ) = e.second;
                }
            return result;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["threshold"] = m_threshold;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters& parameters  ) {
            OptionalParameters::maybeLoadParameter( parameters, "threshold", m_threshold );
            }

    private:
        double m_threshold;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_JACOBIANTHRESHOLDBASEDTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

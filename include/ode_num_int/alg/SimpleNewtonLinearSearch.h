// SimpleNewtonLinearSearch.h

#ifndef _ALG_SIMPLENEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_SIMPLENEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonLinearSearch.h"

namespace ctm {
namespace math {

template< class VD >
class SimpleNewtonLinearSearch :
    public NewtonLinearSearch< VD >,
    public FactoryMixin< SimpleNewtonLinearSearch<VD>, NewtonLinearSearch<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        SimpleNewtonLinearSearch() :
            m_maxStepTruncations( DefaultMaxStepTruncations )
            {}

        real_type lineSearch( const V& dir, const V& x0, const V& f0, V *residual = nullptr )
            {
            unsigned int iteration = 0;
            auto& m = *this->mapping();
            real_type f0Norm2 = f0.euclideanNormSquare();

            real_type alpha = 1;
            real_type alphaBest = 1;
            real_type f_f0Best = 1;
            const real_type eta = 0.8;
            unsigned int itrunc;
            auto makeNextState = [&]( real_type beta ) -> V {
                V x = x0;
                V dx = dir;
                dx *= beta;
                x += dx;
                return x;
                };
            V f;
            auto rhsNormSquare = [&]( real_type beta ) -> real_type {
                f = m( makeNextState( beta ) );
                this->lineSearchObservers( iteration++, beta, dir, x0, f0, f );
                return f.euclideanNormSquare();
                };
            for( itrunc=0; itrunc < m_maxStepTruncations; ++itrunc, alpha*=0.5 ) {
                real_type fNorm2 = rhsNormSquare( alpha );
                real_type f_f0 = fNorm2 / f0Norm2;
                if( f_f0Best > f_f0 ) {
                    f_f0Best = f_f0;
                    alphaBest = alpha;
                    if( residual )
                        *residual = f;
                    }
                if( f_f0Best < eta )
                    break;
                }
            if( itrunc == m_maxStepTruncations   &&   f_f0Best == 1 ) {
                // cout << "No convergence within max. step truncations, trying the opposite direction" << endl;
                alpha = alphaBest = -1;
                for( itrunc=0; itrunc < m_maxStepTruncations; ++itrunc, alpha*=0.5 ) {
                    real_type fNorm2 = rhsNormSquare( alpha );
                    real_type f_f0 = fNorm2 / f0Norm2;
                    if( f_f0Best > f_f0 ) {
                        f_f0Best = f_f0;
                        alphaBest = alpha;
                        if( residual )
                            *residual = f;
                        }
                    if( f_f0Best < eta )
                        break;
                    }
                if( itrunc == m_maxStepTruncations   &&   f_f0Best == 1 ) {
                    // cout << "In the opposite direction, there is still no convergence within max. step truncations" << endl;
                    alphaBest = 0;
                    }
                }
            return alphaBest;
            }

        unsigned int maxStepTruncations() const {
            return m_maxStepTruncations;
            }

        void setMaxStepTruncations( unsigned int maxStepTruncations ) {
            m_maxStepTruncations = maxStepTruncations;
            }

    private:
        const unsigned int DefaultMaxStepTruncations = 10;
        unsigned int m_maxStepTruncations;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_SIMPLENEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// JacobianBroydenUpdateNewtonDescentDirection.h

#ifndef _ALG_JACOBIANBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_JACOBIANBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonDescentDirection.h"
#include "../lu/LUFactorizer.h"

namespace ctm {
namespace math {

// The Jacobian is computed for the first time;
// then it is updated each next time using the "Broyden good" formula;
// linear system is solved to find the direction.
// The main downside is that the Jacobian becomes dense - do not use in production code!
template< class VD >
class JacobianBroydenUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianBroydenUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianBroydenUpdateNewtonDescentDirection() :
            m_havePrev( false )
            {}

        void reset( bool hard )
            {
            if( hard )
                this->jacobianProvider()->hardReset();
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            auto& J = jp->jacobian();
            auto recalcJacobian = J.empty();
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                jp->computeJacobian( x0 );
                this->jacobianRefreshObservers( false );
                }
            this->ddirPreObservers();
            if( !recalcJacobian   &&   m_havePrev ) {
                V dx = x0 - m_xprev;
                V df = f0 - m_fprev;
                V left = df - J*dx;
                left *= 1 / dx.euclideanNormSquare();
                unsigned int n = x0.size();
                ASSERT( J.size() == sparse::SparseMatrixCommonTypes::Index(n, n) );
                for( unsigned int i=0; i<n; ++i )
                    for( unsigned int j=0; j<n; ++j ) {
                        real_type x = left[i] * dx[j];
                        if( x != 0 )
                            J.at( i, j ) += x;
                        }
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            this->jacobianObservers( J, x0, f0 );
            LUFactorizer<real_type> lu( J );
            dir = f0;
            lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += lu.timingStats();
            }

    private:
        V m_xprev;
        V m_fprev;
        bool m_havePrev;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_JACOBIANBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

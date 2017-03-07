// JacobianFakeBroydenUpdateNewtonDescentDirection.h

#ifndef _ALGSOLVER_JACOBIANFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_JACOBIANFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "NewtonDescentDirection.h"
#include "../lu/LUFactorizer.h"

namespace ctm {
namespace math {

template< class VD >
class JacobianFakeBroydenUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianFakeBroydenUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianFakeBroydenUpdateNewtonDescentDirection() :
            m_havePrev( false )
            {}

        void reset( bool hard )
            {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
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
                for( auto& element : J )
                    element.second += left[element.first.first] * dx[element.first.second];
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            this->jacobianObservers( J, x0, f0 );
            if( m_lu.empty() )
                m_lu.setMatrix( J );
            else
                m_lu.setMatrixFast( J );
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
        V m_xprev;
        V m_fprev;
        bool m_havePrev;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_JACOBIANFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

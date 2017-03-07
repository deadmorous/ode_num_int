// JacobianHartUpdateNewtonDescentDirection.h

#ifndef _ALG_JACOBIANHARTUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_JACOBIANHARTUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonDescentDirection.h"
#include "../lu/util.h"

namespace ctm {
namespace math {

// The Jacobian is computed for the first time;
// then it is updated each next time using the Hart formula, see LUFactorizer::secantUpdateHart().
// Linear system is solved to find the direction.
template< class VD >
class JacobianHartUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianHartUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianHartUpdateNewtonDescentDirection() :
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
            bool recalcJacobian = !m_lu.isFactorized();
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                auto jp = this->jacobianProvider();
                ASSERT( jp );
                auto& J = jp->jacobian();
                jp->computeJacobian( x0 );
                m_lu.setMatrix( J );
                this->jacobianRefreshObservers( false );
                }
            this->ddirPreObservers();
            if( !recalcJacobian    &&   m_havePrev ) {
                V s = x0 - m_xprev;
                V y = f0 - m_fprev;
                m_lu.secantUpdateHart( &*s.begin(), &*y.begin() );
                // m_lu.secantUpdateProportional( &*s.begin(), &*y.begin() );

                // deBUG, TODO: Remove
                /*
                V x = y;
                m_lu.solve( &*x.begin() );
                x -= s;
                real_type relError = x.infNorm() / s.infNorm();
                if ( relError > 1e-8 ) {
                    int a = 123;
                    }
                //*/
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            if( !this->jacobianObservers.empty() )
                this->jacobianObservers( luToMatrix(m_lu), x0, f0 );
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

#endif // _ALG_JACOBIANHARTUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// ConstJacobianNewtonDescentDirection.h

#ifndef _ALG_CONSTJACOBIANNEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_CONSTJACOBIANNEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonDescentDirection.h"
#include "../lu/LUFactorizer.h"

namespace ctm {
namespace math {

// The Jacobian is only computed and factorized once - at the beginning.
template< class VD >
class ConstJacobianNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< ConstJacobianNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        void reset( bool hard ) {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            if( !m_lu.isFactorized() ) {
                this->jacobianRefreshObservers( true );
                auto jp = this->jacobianProvider();
                ASSERT( jp );
                auto& J = jp->jacobian();
                jp->computeJacobian( x0 );
                m_lu.setMatrix( J );
                this->jacobianRefreshObservers( false );
                }
            if( !this->jacobianObservers.empty() )
                this->jacobianObservers( luToMatrix(m_lu), x0, f0 );
            this->ddirPreObservers();
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_CONSTJACOBIANNEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

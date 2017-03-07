// SimpleNewtonDescentDirection.h

#ifndef _ALG_SIMPLENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_SIMPLENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonDescentDirection.h"
#include "../lu/LUFactorizer.h"

namespace ctm {
namespace math {

// The Jacobian is recomputed each time; linear system is solved to find the direction
template< class VD >
class SimpleNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< SimpleNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
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
            return this->jacobianProvider()->hardResetMayHelp();
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            this->jacobianRefreshObservers( true );
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            jp->computeJacobian( x0 );
            auto& J = jp->jacobian();
            this->jacobianRefreshObservers( false );
            this->jacobianObservers( J, x0, f0 );
            this->ddirPreObservers();
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
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_SIMPLENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

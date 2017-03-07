// ode_model_coupled_nl_osc.h

#ifndef _ODE_MODEL_COUPLED_NL_OSC_H_
#define _ODE_MODEL_COUPLED_NL_OSC_H_

#include "../interfaces/OdeRhs.h"

namespace ctm {
namespace testmodels {

using namespace math;

template< class VD >
class CoupledNonlinearOscillators :
    public OdeRhs< VD >,
    public FactoryMixin< CoupledNonlinearOscillators<VD>, OdeRhs<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        explicit CoupledNonlinearOscillators( unsigned int count = 10, real_type c0 = 1e3, real_type c2 = 1e3, real_type f = 100 ) :
            m_count( count ),
            m_c0( c0 ),
            m_c2( c2 ),
            m_f( f )
            {}

        virtual unsigned int secondOrderVarCount() const {
            return m_count;
            }

        virtual unsigned int firstOrderVarCount() const {
            return 0;
            }

        virtual void rhs( V& dst, real_type time, const V& x ) const
            {
            this->odeRhsPreObservers( time, x, this );
            dst.resize( 2*m_count );
            for( unsigned int i=0; i<m_count; ++i ) {
                dst[i] = x[m_count + i];
                real_type dprev = x[i];
                if( i > 0 )
                    dprev -= x[i-1];
                real_type f = -force( dprev );
                if( i+1 < m_count )
                    f += force( x[i+1] - x[i] );
                else if( i+1 == m_count )
                    f += m_f;
                dst[m_count+i] = f;
                }
            this->odeRhsPostObservers( time, x, dst, this );
            }

        Parameters parameters() const
            {
            Parameters result;
            result["n"] = m_count;
            result["c0"] = m_c0;
            result["c2"] = m_c2;
            result["f"] = m_f;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "n", m_count );
            this->maybeLoadParameter( parameters, "c0", m_c0 );
            this->maybeLoadParameter( parameters, "c2", m_c2 );
            this->maybeLoadParameter( parameters, "f", m_f );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["n"] = "Number of oscillators";
            result["c0"] = "Coefficient at x^2/2 in potential energy (linear stiffness)";
            result["c2"] = "Coefficient at x^4/4 in potential energy (cubic stiffness)";
            result["f"] = "Constant force due to which there are oscillations with zero initial state";
            return result;
            }

        std::string helpOnType() const {
            return "Chain of coupled nonlinear oscillators. The mass of each oscillator equals 1,\n"
                   "and the elastic characteristic is c0*x^2/2 + c2*x^4/4.\n"
                   "Constant force f is applied in order to have oscillations when the initial\n"
                   "state is zero.";
            }

    private:
        unsigned int m_count;
        real_type m_c0;
        real_type m_c2;
        real_type m_f;
        real_type force( real_type deformation ) const {
            return deformation * ( m_c0 + m_c2*deformation*deformation );
            }
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_MODEL_COUPLED_NL_OSC_H_

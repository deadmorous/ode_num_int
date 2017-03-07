// ode_model_oscillator.h

#ifndef _ODE_MODEL_OSCILLATOR_H_
#define _ODE_MODEL_OSCILLATOR_H_

#include "../interfaces/OdeRhs.h"

namespace ctm {
namespace testmodels {

using namespace math;

template< class VD >
class Oscillator :
    public OdeRhs< VD >,
    public FactoryMixin< Oscillator<VD>, OdeRhs<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        explicit Oscillator( real_type c = 1e3, real_type f = 100 ) :
            m_c( c ),
            m_f( f )
            {}

        virtual unsigned int secondOrderVarCount() const {
            return 1;
            }

        virtual unsigned int firstOrderVarCount() const {
            return 0;
            }

        virtual void rhs( V& dst, real_type time, const V& x ) const
            {
            this->odeRhsPreObservers( time, x, this );
            dst.resize( 2 );
            dst[0] = x[1];
            dst[1] = m_f - m_c * x[0];
            this->odeRhsPostObservers( time, x, dst, this );
            }

        Parameters parameters() const
            {
            Parameters result;
            result["c"] = m_c;
            result["f"] = m_f;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "c", m_c );
            this->maybeLoadParameter( parameters, "f", m_f );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["c"] = "Stiffness (note: mass equals 1)";
            result["f"] = "Constant force due to which there are oscillations with zero initial state";
            return result;
            }

        std::string helpOnType() const {
            return "Linear oscillator. The mass equals 1, and the stiffness is specified by the c\n"
                   "parameter. Constant force f is applied in order to have oscillations when the\n"
                   "initial state is zero.";
            }

    private:
        real_type m_c;
        real_type m_f;
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_MODEL_OSCILLATOR_H_

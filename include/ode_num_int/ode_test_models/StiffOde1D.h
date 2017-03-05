// ode_model_stiff_1d.h

#ifndef _ODE_MODEL_STIFF_1D_H_
#define _ODE_MODEL_STIFF_1D_H_

#include "../ode_solver/OdeRhs.h"

namespace ctm {
namespace testmodels {

using namespace math;

template< class VD >
class StiffOde1D :
    public OdeRhs< VD >,
    public FactoryMixin< StiffOde1D<VD>, OdeRhs<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        explicit StiffOde1D( unsigned int a = 50 ) :
            m_a( a )
            {}

        virtual unsigned int secondOrderVarCount() const {
            return 0;
            }

        virtual unsigned int firstOrderVarCount() const {
            return 1;
            }

        virtual void rhs( V& dst, real_type time, const V& x ) const
            {
            this->odeRhsPreObservers( time, x, this );
            dst.resize( 1 );
            dst[0] = -m_a*(x[0] - cos(time));
            this->odeRhsPostObservers( time, x, dst, this );
            }

        Parameters parameters() const
            {
            Parameters result;
            result["a"] = m_a;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "a", m_a );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["a"] = "Coefficient a in the equation y' = -a*(y-cos(x))";
            return result;
            }

        std::string helpOnType() const {
            return "Equation y' = -a*(y-cos(x))";
            }

    private:
        real_type m_a;
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_MODEL_STIFF_1D_H_

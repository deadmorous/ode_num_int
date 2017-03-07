// OdeStepMapping.h

#ifndef _ODESOLVER_ODESTEPMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESTEPMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../alg/interfaces/VectorMapping.h"
#include "OdeRhs.h"

namespace ctm {
namespace math {

template< class VD >
class OdeStepMapping :
    public VectorMapping<VD>,
    public OdeRhsHolder<VD>,
    public Factory< OdeStepMapping<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        virtual real_type initialTime() const = 0;
        virtual const V& initialState() const = 0;
        virtual void setInitialState( real_type initialTime, const V& initialState ) = 0;
        virtual real_type timeStep() const = 0;
        virtual void setTimeStep( real_type timeStep ) = 0;
        virtual V eqnInitialState() const = 0;
        virtual void computeOdeState( V& odeState, const V& eqnState ) const = 0;
        virtual void computeEqnState( V& eqnState, const V& odeState ) const = 0;
        virtual void beforeStep() const = 0;

        V computeOdeState( const V& eqnState ) const
            {
            V odeState;
            computeOdeState( odeState, eqnState );
            return odeState;
            }

        V computeEqnState( const V& odeState ) const
            {
            V eqnState;
            computeEqnState( eqnState, odeState );
            return eqnState;
            }
    };
CTM_DEF_PROP_VD_TEMPLATE_CLASS(
        OdeStepMappingHolder, OdeStepMapping,
        timeStepper, setTimeStepper )

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESTEPMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

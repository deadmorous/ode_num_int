// reg_ode_time_stepper.h

#ifndef _REG_ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ode_solver/SpecifiedOdeInitState.h"
#include "ode_solver/OdeStepMappingEuler.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SpecifiedOdeInitState, "specified" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeStepMappingEuler, "euler" )

namespace math {

template< class VD >
class OdeStepMappingClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SpecifiedOdeInitState, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeStepMappingEuler, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _REG_ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

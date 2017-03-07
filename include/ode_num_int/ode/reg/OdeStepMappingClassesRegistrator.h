// OdeStepMappingClassesRegistrator.h

#ifndef _ODE_REG_ODESTEPMAPPINGCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_REG_ODESTEPMAPPINGCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../SpecifiedOdeInitState.h"
#include "../OdeStepMappingEuler.h"

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

#endif // _ODE_REG_ODESTEPMAPPINGCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

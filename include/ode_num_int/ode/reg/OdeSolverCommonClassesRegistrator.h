// OdeSolverCommonClassesRegistrator.h

#ifndef _ODE_REG_ODESOLVERCOMMONCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_REG_ODESOLVERCOMMONCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../ErrorInfNormCalculator.h"
#include "../OdeStepSizeSimpleController.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::ErrorInfNormCalculator, "inf" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeStepSizeSimpleController, "simple" )

namespace math {

template< class VD >
class OdeSolverCommonClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( ErrorInfNormCalculator, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeStepSizeSimpleController, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_REG_ODESOLVERCOMMONCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

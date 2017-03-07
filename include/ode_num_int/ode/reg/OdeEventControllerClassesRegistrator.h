// OdeEventControllerClassesRegistrator.h

#ifndef _ODE_REG_ODEEVENTCONTROLLERCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_REG_ODEEVENTCONTROLLERCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../LinearOdeEventController.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::LinearOdeEventController, "linear" )

namespace math {

template< class VD >
class OdeEventControllerClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( LinearOdeEventController, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_REG_ODEEVENTCONTROLLERCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

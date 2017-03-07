// reg_ode_event_controller.h

#ifndef _REG_REG_ODE_EVENT_CONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_REG_ODE_EVENT_CONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ode/LinearOdeEventController.h"

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

#endif // _REG_REG_ODE_EVENT_CONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// reg_ode_solvers.h

#ifndef _REG_ODE_SOLVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ODE_SOLVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../OdeSolverExplicitEuler.h"
#include "../OdeSolverRK4.h"
#include "../OdeSolverDOPRI45.h"
#include "../OdeSolverDOPRI56.h"
#include "../OdeSolverDOPRI78.h"
#include "../OdeSolverRosenbrock_W1.h"
#include "../OdeSolverRosenbrock_SW2_4.h"
#include "../OdeSolverGragg.h"
#include "../OdeSolverExtrapolator.h"
#include "../OdeSolverImplicitEuler.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverExplicitEuler, "euler" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverRK4, "rk4" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverDOPRI45, "dopri_45" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverDOPRI56, "dopri_56" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverDOPRI78, "dopri_78" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverRosenbrock_W1, "rosenbrock_w1" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverRosenbrock_SW2_4, "rosenbrock_sw2_4" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverGragg, "gragg" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverExtrapolator, "extrapolator" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverImplicitEuler, "i_euler" )

namespace math {

template< class VD >
class OdeSolverClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverExplicitEuler, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverRK4, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverDOPRI45, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverDOPRI56, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverDOPRI78, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverRosenbrock_W1, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverRosenbrock_SW2_4, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverGragg, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverExtrapolator, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverImplicitEuler, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _REG_ODE_SOLVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// reg_ode_solvers.h

#ifndef _REG_ODE_SOLVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ODE_SOLVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ode_explicit_euler.h"
#include "ode_rk4.h"
#include "ode_dopri_45.h"
#include "ode_dopri_56.h"
#include "ode_dopri_78.h"
#include "ode_rosenbrock_w.h"
#include "ode_gragg.h"
#include "ode_extrapolator.h"
#include "ode_implicit_euler.h"

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

CTM_DECL_IMPLEMENTATION_TRAITS( math::ExtrapolatorStepSequenceRomberg, "romberg" )
CTM_DECL_IMPLEMENTATION_TRAITS( math::ExtrapolatorStepSequenceBulirsch, "bulirsch" )
CTM_DECL_IMPLEMENTATION_TRAITS( math::ExtrapolatorStepSequenceHarmonic, "harmonic" )

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

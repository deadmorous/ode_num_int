// ode_solvers.cpp

#include "ode_explicit_euler.h"
#include "ode_rk4.h"
#include "ode_dopri_45.h"
#include "ode_dopri_56.h"
#include "ode_dopri_78.h"
#include "ode_rosenbrock_w.h"
#include "ode_gragg.h"
#include "ode_extrapolator.h"
#include "ode_implicit_euler.h"
#include "defs.h"

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverExplicitEuler, "euler" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverRK4, "rk4" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverDOPRI45, "dopri_45" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverDOPRI56, "dopri_56" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverDOPRI78, "dopri_78" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverRosenbrock_W1, "rosenbrock_w1" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverRosenbrock_SW2_4, "rosenbrock_sw2_4" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverGragg, "gragg" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverExtrapolator, "extrapolator" )

CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceRomberg, "romberg" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceBulirsch, "bulirsch" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceHarmonic, "harmonic" )

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverImplicitEuler, "i_euler" )

} // end namespace math
} // end namespace ctm

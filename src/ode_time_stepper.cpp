// ode_time_stepper.cpp

#include "ode_time_stepper.h"
#include "vector_for_ode_solver.h"

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( SpecifiedOdeInitState, "specified" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( FakeDatFileOdeInitState, "sim_dyn_initial" )

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeTimeStepperEuler, "euler" )

} // end namespace math
} // end namespace ctm

// ode_test_models.cpp

#include "ode_test_models.h"
#include "vector_for_ode_solver.h"

namespace ctm {
namespace testmodels {

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( Oscillator, "oscillator" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( CoupledNonlinearOscillators, "coupled_nl_osc" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( StiffOde1D, "stiff_1d" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( SlidingPoint, "sliding_point" )

} // end namespace testmodels
} // end namespace ctm

// reg_ode_solvers.cpp

#include "ode_solver/ExtrapolatorStepSequenceRomberg.h"
#include "ode_solver/ExtrapolatorStepSequenceBulirsch.h"
#include "ode_solver/ExtrapolatorStepSequenceHarmonic.h"

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceRomberg, "romberg" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceBulirsch, "bulirsch" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceHarmonic, "harmonic" )

} // end namespace math
} // end namespace ctm

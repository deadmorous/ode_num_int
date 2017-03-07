// reg_ode_solvers.cpp

#include "ode/ExtrapolatorStepSequenceRomberg.h"
#include "ode/ExtrapolatorStepSequenceBulirsch.h"
#include "ode/ExtrapolatorStepSequenceHarmonic.h"

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceRomberg, "romberg" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceBulirsch, "bulirsch" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceHarmonic, "harmonic" )

} // end namespace math
} // end namespace ctm

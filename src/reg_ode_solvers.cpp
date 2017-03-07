// reg_ode_solvers.cpp

#include "ode_num_int/ExtrapolatorStepSequenceRomberg.h"
#include "ode_num_int/ExtrapolatorStepSequenceBulirsch.h"
#include "ode_num_int/ExtrapolatorStepSequenceHarmonic.h"

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceRomberg, "romberg" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceBulirsch, "bulirsch" )
CTM_FACTORY_REGISTER_TYPE( ExtrapolatorStepSequenceHarmonic, "harmonic" )

} // end namespace math
} // end namespace ctm

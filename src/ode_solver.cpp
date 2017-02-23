// ode_solver.cpp

#include "ode_solver.h"
#include "defs.h"
#include "foreachindex.h"

#include <fstream>
#include <sstream>
#include <set>

namespace ctm {
namespace math {

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( ErrorInfNormCalculator, "inf" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeStepSizeSimpleController, "simple" )

} // end namespace math
} // end namespace ctm

// ode_solver_output.cpp

#include "ode_solver_output.h"
#include "vector_for_ode_solver.h"

namespace ctm {
namespace tools {
namespace cmd {
namespace numint {

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverStatisticsOutput, "stats_full" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverStepGeneralConsoleOutput, "con_general" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverStepSolutionConsoleOutput, "con_solution" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverStepSolutionColumnwiseOutput, "con_solution_columnwise" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeSolverSimDynFileOutput, "solution_simout" )

} // end namespace numint
} // end namespace cmd
} // end namespace tools
} // end namespace ctm

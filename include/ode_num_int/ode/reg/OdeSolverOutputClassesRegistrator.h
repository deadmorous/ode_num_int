// OdeSolverOutputClassesRegistrator.h

#ifndef _ODE_REG_ODESOLVEROUTPUTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_REG_ODESOLVEROUTPUTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../ode_solver_output.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverStatisticsOutput, "stats_full" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverStepGeneralConsoleOutput, "con_general" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverStepSolutionConsoleOutput, "con_solution" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::OdeSolverStepSolutionColumnwiseOutput, "con_solution_columnwise" )

namespace math {

template< class VD >
class OdeSolverOutputClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverStatisticsOutput, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverStepGeneralConsoleOutput, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverStepSolutionConsoleOutput, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( OdeSolverStepSolutionColumnwiseOutput, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_REG_ODESOLVEROUTPUTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

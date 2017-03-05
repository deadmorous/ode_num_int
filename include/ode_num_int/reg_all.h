// reg_all.h

#ifndef _REG_ALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "reg_algebraic_eqn_solver.h"
#include "reg_ode_event_controller.h"
#include "reg_ode_solver.h"
#include "reg_ode_solvers.h"
#include "reg_ode_time_stepper.h"
#include "reg_ode_solver_output.h"

namespace ctm {
namespace math {

template< class VD >
class OdeNumIntClassesRegistrator
    {
    private:
        AlgebraicSolverClassesRegistrator<VD> m_algebraicSolverClassesRegistrator;
        OdeEventControllerClassesRegistrator<VD> m_odeEventControllerClassesRegistrator;
        OdeSolverCommonClassesRegistrator<VD> m_odeSolverCommonClassesRegistrator;
        OdeSolverClassesRegistrator<VD> m_odeSolverClassesRegistrator;
        OdeStepMappingClassesRegistrator<VD> m_odeStepMappingClassesRegistrator;
        OdeSolverOutputClassesRegistrator<VD> m_odeSolverOutputClassesRegistrator;
    };


} // end namespace math
} // end namespace ctm

#endif // _REG_ALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// OdeNumIntClassesRegistrator.h

#ifndef _ODENUMINTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODENUMINTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./alg/reg/AlgebraicSolverClassesRegistrator.h"
#include "./ode/reg/OdeEventControllerClassesRegistrator.h"
#include "./ode/reg/OdeSolverCommonClassesRegistrator.h"
#include "./ode/reg/OdeSolverClassesRegistrator.h"
#include "./ode/reg/OdeStepMappingClassesRegistrator.h"
#include "./ode/reg/OdeSolverOutputClassesRegistrator.h"

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

#endif // _ODENUMINTCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

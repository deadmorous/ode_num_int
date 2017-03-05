// reg_algebraic_eqn_solver.h

#ifndef _REG_ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "algebraic_eqn_solver.h"
#include "alg_solver/DefaultJacobianProvider.h"
#include "alg_solver/SparseJacobianProvider.h"
#include "alg_solver/SimpleErrorEstimator.h"
#include "alg_solver/SimpleNewtonDescentDirection.h"
#include "alg_solver/JacobianWidthBasedTrimmer.h"
#include "alg_solver/JacobianThresholdBasedTrimmer.h"
#include "alg_solver/JacobianBroydenUpdateNewtonDescentDirection.h"
#include "alg_solver/JacobianFakeBroydenUpdateNewtonDescentDirection.h"
#include "alg_solver/JacobianLazyFakeBroydenUpdateNewtonDescentDirection.h"
#include "alg_solver/JacobianHartUpdateNewtonDescentDirection.h"
#include "alg_solver/ConstJacobianNewtonDescentDirection.h"
#include "alg_solver/SimpleNewtonLinearSearch.h"
#include "alg_solver/NewtonIterationPerformerImpl.h"
#include "alg_solver/NewtonSolver.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::DefaultJacobianProvider, "default" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SparseJacobianProvider, "sparse" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SimpleErrorEstimator, "simple" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SimpleNewtonDescentDirection, "simple" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianWidthBasedTrimmer, "width" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianThresholdBasedTrimmer, "threshold" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianBroydenUpdateNewtonDescentDirection, "broyden" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianFakeBroydenUpdateNewtonDescentDirection, "fake-broyden" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianLazyFakeBroydenUpdateNewtonDescentDirection, "lazy-fake-broyden" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianHartUpdateNewtonDescentDirection, "hart" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::ConstJacobianNewtonDescentDirection, "const" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SimpleNewtonLinearSearch, "simple" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::NewtonIterationPerformerImpl, "default" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::NewtonSolver, "newton" )

namespace math {

template< class VD >
class AlgebraicSolverClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( DefaultJacobianProvider, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SparseJacobianProvider, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SimpleErrorEstimator, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SimpleNewtonDescentDirection, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianWidthBasedTrimmer, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianThresholdBasedTrimmer, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianBroydenUpdateNewtonDescentDirection, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianFakeBroydenUpdateNewtonDescentDirection, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianLazyFakeBroydenUpdateNewtonDescentDirection, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianHartUpdateNewtonDescentDirection, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( ConstJacobianNewtonDescentDirection, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SimpleNewtonLinearSearch, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( NewtonIterationPerformerImpl, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( NewtonSolver, VD )
    };

} // end namespace math
} // end namespace ctm

#endif // _REG_ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

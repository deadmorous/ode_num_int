// reg_algebraic_eqn_solver.h

#ifndef _REG_ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _REG_ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../DefaultJacobianProvider.h"
#include "../SparseJacobianProvider.h"
#include "../SimpleErrorEstimator.h"
#include "../SimpleNewtonDescentDirection.h"
#include "../JacobianWidthBasedTrimmer.h"
#include "../JacobianThresholdBasedTrimmer.h"
#include "../JacobianBroydenUpdateNewtonDescentDirection.h"
#include "../JacobianFakeBroydenUpdateNewtonDescentDirection.h"
#include "../JacobianLazyFakeBroydenUpdateNewtonDescentDirection.h"
#include "../JacobianHartUpdateNewtonDescentDirection.h"
#include "../ConstJacobianNewtonDescentDirection.h"
#include "../SimpleNewtonLinearSearch.h"
#include "../NewtonIterationPerformerImpl.h"
#include "../NewtonSolver.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::DefaultJacobianProvider, "default" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SparseJacobianProvider, "sparse" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SimpleErrorEstimator, "simple" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianWidthBasedTrimmer, "width" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::JacobianThresholdBasedTrimmer, "threshold" )

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( math::SimpleNewtonDescentDirection, "simple" )
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

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianWidthBasedTrimmer, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( JacobianThresholdBasedTrimmer, VD )

        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SimpleNewtonDescentDirection, VD )
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

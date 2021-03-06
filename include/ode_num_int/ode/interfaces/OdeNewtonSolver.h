// OdeNewtonSolver.h

#ifndef _ODE_INTERFACES_ODENEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODENEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeSolver.h"
#include "../../alg/interfaces/NewtonSolverInterface.h"

namespace ctm {
namespace math {

template< class VD >
class OdeNewtonSolver :
    public OdeSolverComponent<VD>,
    public NewtonSolverHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeNewtonSolver( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setNewtonSolver( Factory< NewtonSolverInterface<VD> >::newInstance( typeId ) );
            }

        OdeNewtonSolver( const OdeNewtonSolver<VD>& ) = delete;
        OdeNewtonSolver<VD>& operator=( const OdeNewtonSolver<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["algsolver"] = this->newtonSolver();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "algsolver",
                        std::bind( &OdeNewtonSolver<VD>::setNewtonSolver, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["algsolver"] = OptionalParameters::appendNestedHelp(
                        this->newtonSolver(),
                        "Algebraic equation solver" );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODENEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

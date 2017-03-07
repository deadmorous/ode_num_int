// OdeSolverErrorNormCalculator.h

#ifndef _ODE_INTERFACES_ODESOLVERERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODESOLVERERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./ErrorNormCalculator.h"
#include "./OdeSolver.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverErrorNormCalculator :
    public OdeSolverComponent<VD>,
    public ErrorNormCalculatorHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverErrorNormCalculator( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            auto updateOdeRhs = [this] {
                this->errorNormCalculator()->setOdeRhs( this->solver().odeRhs() );
                };
            this->onErrorNormCalculatorChanged( updateOdeRhs );
            solver.onOdeRhsChanged( updateOdeRhs );
            if( !typeId.empty() )
                this->setErrorNormCalculator( Factory< ErrorNormCalculator<VD> >::newInstance( typeId ) );
            }

        OdeSolverErrorNormCalculator( const OdeSolverErrorNormCalculator<VD>& ) = delete;
        OdeSolverErrorNormCalculator<VD>& operator=( const OdeSolverErrorNormCalculator<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["enorm"] = this->errorNormCalculator();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "enorm",
                        std::bind( &ErrorNormCalculatorHolder<VD>::setErrorNormCalculator, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["enorm"] = OptionalParameters::appendNestedHelp(
                        this->errorNormCalculator(),
                        "Error norm calculator" );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODESOLVERERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

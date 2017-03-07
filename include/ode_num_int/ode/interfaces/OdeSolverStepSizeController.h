// OdeSolverStepSizeController.h

#ifndef _ODE_INTERFACES_ODESOLVERSTEPSIZECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODESOLVERSTEPSIZECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeStepSizeController.h"
#include "./OdeSolver.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverStepSizeController :
    public OdeSolverComponent<VD>,
    public OdeStepSizeControllerHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverStepSizeController( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setOdeStepSizeController( Factory< OdeStepSizeController<VD> >::newInstance( typeId ) );
            }

        OdeSolverStepSizeController( const OdeSolverStepSizeController<VD>& ) = delete;
        OdeSolverStepSizeController<VD>& operator=( const OdeSolverStepSizeController<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["stepsizectl"] = this->odeStepSizeController();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "stepsizectl",
                        std::bind( &OdeStepSizeControllerHolder<VD>::setOdeStepSizeController, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["stepsizectl"] = OptionalParameters::appendNestedHelp(
                        this->odeStepSizeController(),
                        "Step size control algorithm" );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODESOLVERSTEPSIZECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

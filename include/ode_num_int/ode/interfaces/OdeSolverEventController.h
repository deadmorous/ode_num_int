// OdeSolverEventController.h

#ifndef _ODE_INTERFACES_ODESOLVEREVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODESOLVEREVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeEventController.h"
#include "./OdeSolver.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverEventController :
    public OdeSolverComponent<VD>,
    public OdeEventControllerHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverEventController( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setEventController( Factory< OdeEventController<VD> >::newInstance( typeId ) );

            auto setControllerRhs = [this]() {
                if( auto ec = this->eventController() )
                    ec->setOdeRhs( this->solver().odeRhs() );
                };
            this->onEventControllerChanged( setControllerRhs );
            solver.onOdeRhsChanged( setControllerRhs );
            }

        OdeSolverEventController( const OdeSolverEventController<VD>& ) = delete;
        OdeSolverEventController<VD>& operator=( const OdeSolverEventController<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["eventctl"] = this->eventController();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "eventctl",
                        std::bind( &OdeEventControllerHolder<VD>::setEventController, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["eventctl"] = OptionalParameters::appendNestedHelp(
                        this->eventController(),
                        "Event controller algorithm" );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODESOLVEREVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

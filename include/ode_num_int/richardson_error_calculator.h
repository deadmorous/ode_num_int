// richardson_error_calculator.h

#ifndef _RICHARDSON_ERROR_CALCULATOR_H_
#define _RICHARDSON_ERROR_CALCULATOR_H_

#include "ode_solver/ErrorNormCalculator.h"
#include "ode_solver/OdeStepSizeController.h"
#include "ode_solver/OdeEventController.h"
#include "ode_solver/OdeSolver.h"
#include "error_stats.h"

namespace ctm {
namespace math {

template< class VD >
using RichardsonErrorCalculatorStepObservers = cxx::Observers<
    typename VD::value_type /*time*/,
    typename VD::value_type /*stepSize*/,
    const VectorTemplate<VD>& /*error*/ >;

template< class VD >
class RichardsonErrorCalculator :
    public OdeSolverHolder<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        RichardsonErrorCalculatorStepObservers<VD> stepObservers;

        ErrorStats<V> calculateError( real_type T, real_type h ) const
            {
            auto solver = this->odeSolver();
            OptionalParameters::Parameters solverParam;
            solverParam["disable_error_check"] = 1;
            solver->setParameters( solverParam );

            auto x = solver->initialState();
            auto n = x.size();
            auto t = solver->initialTime();
            auto tEnd = t + T;
            ErrorStats<V> result = ErrorStats<V>( V(n) );
            V error( n );
            int p = solver->order();
            real_type factor = real_type( 1 << p ) / ( ( 1 << p ) - 1 );

            bool skipStep = false;
            auto scopedOdeRhsPreCb = cxx::makeScopedIdentifiedElement(
                        solver->odeSolverPostObservers, [&](
                            real_type h,
                            bool stepAccepted,
                            bool stepSizeChanged,
                            bool stepTruncated,
                            real_type errorNorm,
                            unsigned int /*izfTrunc*/,
                            int /*transitionType*/,
                            const OdeSolver<VD>* s )
                {
                    if( !stepAccepted   ||   stepTruncated )
                        skipStep = true;
                    if( stepSizeChanged )
                        throw cxx::exception( "ODE solver changed step size, which is currently unexpected" );
                } );
            do {
                skipStep = false;
                solver->setInitialState( t, x, true );
                solver->setInitialStepSize( 0.5*h );
                solver->doStep();
                solver->doStep();
                auto x1 = solver->initialState();

                solver->setInitialState( t, x, true );
                solver->setInitialStepSize( h );
                solver->doStep();
                auto x2 = solver->initialState();

                if( !skipStep ) {
                    error = x1;
                    error -= x2;
                    error *= factor;
                    result.addSample( error );
                    stepObservers( t, h, error );
                    }

                x = x1;
                t += h;
                }
            while( t < tEnd );
            return result;
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _RICHARDSON_ERROR_CALCULATOR_H_

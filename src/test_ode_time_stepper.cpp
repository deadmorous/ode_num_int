// test_ode_time_stepper.cpp

#include "ode_test_models.h"
#include "alg_solver/JacobianHartUpdateNewtonDescentDirection.h"
#include "ode_solver/OdeStepMappingEuler.h"
#include "alg_solver/NewtonSolver.h"

#include <iostream>

using namespace std;
using namespace ctm;
using namespace math;
using namespace testmodels;

namespace {

void testOdeStepMapping()
    {
    typedef VectorData<double> MyVD;
    typedef VectorTemplate< MyVD > MyVec;
    typedef CoupledNonlinearOscillators< MyVD > MyOdeRhs;

    auto cb = []( unsigned int iterationNumber, const NewtonSolverInterface<MyVD>* solver ) -> bool
        {
        auto itperf = solver->iterationPerformer();
        auto alpha = itperf->currentStepRatio();
        auto x = itperf->currentSolution();
        auto statusMsg = itperf->errorEstimator()->currentStatusMessage();
        cout << "Iteration " << ( iterationNumber + 1 )
             << ", alpha=" << alpha
             << ", " << statusMsg
             << ", x=" << x.toString(3) << endl;
        return false;
        };

    auto odeRhs = make_shared< MyOdeRhs >( 10, 1e3, 3e4, 100 );
    auto eqnRhs = make_shared< OdeStepMappingEuler<MyVD> >();
    eqnRhs->setOdeRhs( odeRhs );
    eqnRhs->setImplicitnessParameter( 1 );
    eqnRhs->setInitialState( 0, MyVec( odeRhs->varCount() ) );
    eqnRhs->setTimeStep( 0.03 );

    NewtonSolver<MyVD> solver;
    solver.setComponent( eqnRhs );
    // solver.setComponent( make_shared< SimpleNewtonDescentDirection<MyVD> >() );
    // solver.setComponent( make_shared< JacobianBroydenUpdateNewtonDescentDirection<MyVD> >() );
    // solver.setComponent( make_shared< JacobianFakeBroydenUpdateNewtonDescentDirection<MyVD> >() );
    solver.setComponent( make_shared< JacobianHartUpdateNewtonDescentDirection<MyVD> >() );
    // solver.setComponent( make_shared< ConstJacobianNewtonDescentDirection<MyVD> >() );

    // TODO: NewtonRegularizationStrategy
    MyVec x0( eqnRhs->inputSize() );
    solver.setInitialGuess( x0 );
    auto scopedObserver = makeScopedIdentifiedElement( solver.iterationObservers, cb );
    double time = 0;
    for (unsigned int timeStep=0; timeStep<10; ++timeStep) {
        cout << "=== Time step " << (timeStep + 1) << " ===" << endl;
        auto status = solver.run();
        cout << solver.statusText( status ) << endl;
        if( status != NewtonSolver<MyVD>::Converged )
            break;
        time += eqnRhs->timeStep();
        eqnRhs->setInitialState( time, eqnRhs->computeOdeState( solver.currentSolution() ) );
        // solver.iterationPerformer()->newtonDescentDirection()->reset( true );
        }
    }

// Uncomment to call the test at startup
// struct _{_() { testOdeStepMapping(); } }__;

} // anonymous namespace

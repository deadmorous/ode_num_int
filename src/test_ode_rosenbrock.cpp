// test_ode_rosenbrock.cpp

#include "ode_num_int/OdeSolverRosenbrock_SW2_4.h"
#include "ode_num_int/CoupledNonlinearOscillators.h"
#include "ode_num_int/ErrorInfNormCalculator.h"
#include "ode_num_int/OdeStepSizeSimpleController.h"

#include <iostream>

using namespace std;
using namespace ctm;
using namespace math;
using namespace testmodels;

namespace {

void testOdeRosenbrockSolver()
    {
    typedef VectorData<double> MyVD;
    typedef VectorTemplate< MyVD > MyVec;
    OdeSolverRosenbrock_SW2_4<MyVD> solver;
    // OdeSolverDOPRI45<MyVD> solver;

    //*
    auto nosc = 2;
    vector<int> cols = nosc > 1? vector<int>{0,nosc-1,nosc,2*nosc-1}: vector<int>{0,1};
    auto odeRhs = make_shared< CoupledNonlinearOscillators<MyVD> >(nosc);
    /*/
    auto odeRhs = make_shared<MyEquation_oscillator>();
    vector<int> cols = {0,1};
    //*/

    solver.setOdeRhs( odeRhs );
    solver.setErrorNormCalculator( make_shared< ErrorInfNormCalculator<MyVD> >() );
    solver.setOdeStepSizeController( make_shared< OdeStepSizeSimpleController<MyVD> >() );
    solver.setInitialStepSize( 0.1 );
    solver.setInitialState( 0, MyVec( odeRhs->varCount() ) );

    OptionalParameters::Parameters stepSizeCtlParam;
    stepSizeCtlParam["tolerance"] = 1e-5;
    solver.odeStepSizeController()->setParameters(stepSizeCtlParam);

    OptionalParameters::Parameters solverParam;
    solverParam["d"] = 0.5;
    solver.setParameters(solverParam);

    unsigned int nSteps = 0;
    unsigned int nRejectedSteps = 0;
    unsigned int nStepChanges = 0;
    solver.odeSolverPostObservers.add( [&]( const OdeSolverPostObserverArg<MyVD>& arg ) {
        ++nSteps;
        if( !arg.stepAccepted() )
            ++nRejectedSteps;
        if( arg.stepSizeChanged() )
            ++nStepChanges;
        //*
        cout << "step " << nSteps
             << ", time = " << arg.solver()->initialTime()
             << ", h = " << arg.stepSize()
             << ", err = " << arg.errorNorm()
             << ", " << (arg.stepAccepted()? "accepted": "rejected");
        if( arg.stepSizeChanged() )
            cout << ", step size changed";
        if( arg.stepTruncated() )
            cout << ", step truncated";
        cout << endl;
        /*/
        if( stepAccepted ) {
            // cout << "state = " << s->initialState().toString( 3 ) << endl;
            auto st = s->initialState();
            cout << s->initialTime();
            for( auto col : cols )
                cout << '\t' << st[col];
            cout << endl;
            }
        //*/
        } );

    double T = 1;
    while( solver.initialTime() < T )
        solver.doStep();
    cout << "=====\n"
         << "Total steps: " << nSteps << endl
         << "Rejected steps: " << nRejectedSteps << " (" << 100.*nRejectedSteps/nSteps << "%)" << endl
         << "Step changes: " << nStepChanges << " (" << 100.*nStepChanges/nSteps << "%)" << endl;
    }

// Uncomment to call the test at startup
// struct _{_() { testOdeRosenbrockSolver(); } }__;

} // anonymous namespace

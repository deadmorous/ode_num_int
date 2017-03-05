// test_ode_rosenbrock.cpp

#include "ode_solver/OdeSolverRosenbrock_SW2_4.h"
#include "ode_test_models.h"
#include "ode_solver/ErrorInfNormCalculator.h"
#include "ode_solver/OdeStepSizeSimpleController.h"

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
    solver.odeSolverPostObservers.add( [&](
                                       double h,
                                       bool stepAccepted,
                                       bool stepSizeChanged,
                                       bool stepTruncated,
                                       double errorNorm,
                                       unsigned int /* izfTrunc */,
                                       int /*transitionType*/,
                                       const OdeSolver<MyVD>* s ) {
        ++nSteps;
        if( !stepAccepted )
            ++nRejectedSteps;
        if( stepSizeChanged )
            ++nStepChanges;
        //*
        cout << "step " << nSteps
             << ", time = " << s->initialTime()
             << ", h = " << h
             << ", err = " << errorNorm
             << ", " << (stepAccepted? "accepted": "rejected");
        if( stepSizeChanged )
            cout << ", step size changed";
        if( stepTruncated )
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

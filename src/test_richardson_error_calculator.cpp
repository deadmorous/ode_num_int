// test_richardson_error_calculator.cpp

#include <iostream>
#include "ode_num_int/RichardsonErrorCalculator.h"
#include "ode_num_int/OdeSolverRK4.h"
#include "ode_num_int/CoupledNonlinearOscillators.h"

using namespace std;
using namespace ctm;
using namespace math;
using namespace testmodels;

namespace {

void testRichardsonErrorCalculator()
    {
    typedef VectorData<double> MyVD;
    typedef VectorTemplate< MyVD > MyVec;
    typedef CoupledNonlinearOscillators< MyVD > MyOdeRhs;

    RichardsonErrorCalculator< MyVD > rec;
    auto solver = make_shared< OdeSolverRK4<MyVD> >();
    rec.setOdeSolver( solver );

    auto nosc = 2;
    auto odeRhs = make_shared<MyOdeRhs>( nosc );

    solver->setOdeRhs( odeRhs );
    solver->setInitialState( 0, MyVec(odeRhs->varCount()) );

    unsigned int nSteps = 0;
    solver->odeSolverPostObservers.add([&]( const OdeSolverPostObserverArg<MyVD>& arg ) {
        ++nSteps;
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
        } );

    double T = 1;
    auto est = rec.calculateError( T, 0.01 );

    cout << "=====\n"
         << "Error max value: " << est.errorMaxValue().toString(3) << endl
         << "Error mean value: " << est.errorMeanValue().toString(3) << endl
         << "Error mean sq. dev.: " << est.errorMeanSqareDev().toString(3) << endl;
    }

// Uncomment to call the test at startup
// struct _{_() { testRichardsonErrorCalculator(); } }__;

} // anonymous namespace

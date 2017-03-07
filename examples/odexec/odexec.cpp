#include "ode_num_int/OdeSolverConfiguration.h"
#include "ode_num_int/OdeSolverRK4.h"
#include "reg.h"

#include <iostream>

int main()
    {
    using namespace std;
    using namespace ctm;
    using namespace math;

    registerTypes();

    typedef VectorData<double> VD;
    typedef VectorTemplate<VD> V;

    try {
        OdeSolverConfiguration<VD> cfg;
        cfg.setValue("rhs", "oscillator");
        cfg.setValue("output_con", "con_solution");
        cfg.setValue("time", 1);
        V x0( 2 );
        auto sc = cfg.apply( set<unsigned int>(), 0, x0 );
        solveOde( &cfg, &sc );
        return 0;
    }
    catch( const std::exception& e ) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
        }
    }

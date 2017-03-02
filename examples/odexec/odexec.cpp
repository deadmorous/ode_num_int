#include "ode_num_int/ode_solver_config.h"
#include "ode_num_int/ode_rk4.h"

#include "ode_num_int/reg_all.h"
#include "ode_num_int/reg_ode_test_models.h"

#include <iostream>

int main()
    {
    using namespace std;
    using namespace ctm;
    using namespace math;


    typedef VectorData<double> VD;
    typedef VectorTemplate<VD> V;

    OdeNumIntClassesRegistrator<VD> registrator;
    testmodels::OdeTestModelClassesRegistrator<VD> modelRegistrator;

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

#include "ode_num_int/reg_all.h"
#include "ode_num_int/reg_ode_test_models.h"

void registerTypes()
    {
    using namespace ctm;
    using namespace math;
    using namespace testmodels;

    typedef VectorData<double> VD;
    typedef VectorTemplate<VD> V;

    OdeNumIntClassesRegistrator<VD> registrator;
    OdeTestModelClassesRegistrator<VD> modelRegistrator;
    }

#include "ode_num_int/OdeNumIntClassesRegistrator.h"
#include "ode_num_int/OdeTestModelClassesRegistrator.h"

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

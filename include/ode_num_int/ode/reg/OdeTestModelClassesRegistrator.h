// OdeTestModelClassesRegistrator.h

#ifndef _ODE_REG_ODETESTMODELCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_REG_ODETESTMODELCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../test_models/Oscillator.h"
#include "../test_models/CoupledNonlinearOscillators.h"
#include "../test_models/StiffOde1D.h"
#include "../test_models/SlidingPoint.h"
#include "../test_models/BouncingBall.h"

namespace ctm {

CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( testmodels::Oscillator, "oscillator" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( testmodels::CoupledNonlinearOscillators, "coupled_nl_osc" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( testmodels::StiffOde1D, "stiff_1d" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( testmodels::SlidingPoint, "sliding_point" )
CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( testmodels::BouncingBall, "bouncing_ball" )

namespace testmodels {

template< class VD >
class OdeTestModelClassesRegistrator
    {
    private:
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( Oscillator, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( CoupledNonlinearOscillators, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( StiffOde1D, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( SlidingPoint, VD )
        CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( BouncingBall, VD )
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_REG_ODETESTMODELCLASSESREGISTRATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

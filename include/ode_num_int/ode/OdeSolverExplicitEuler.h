// OdeSolverExplicitEuler.h

#ifndef _ODE_ODESOLVEREXPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODESOLVEREXPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeSolverExplicitRK.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverExplicitEuler :
    public OdeSolverExplicitRK<VD>,
    public FactoryMixin< OdeSolverExplicitEuler<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverExplicitEuler() :
            OdeSolverExplicitRK<VD>(
                typename ExplicitRKHelper<VD>::Coefficients({
                   // a
                   }, {

                   // b
                   1. }, {

                   // c
                   },
                   -1 ),
                   1 )
            {}
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_ODESOLVEREXPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

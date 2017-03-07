// ode_rk4.h

#ifndef _ODESOLVER_ODESOLVERRK4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERRK4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolverExplicitRK.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverRK4 :
    public OdeSolverExplicitRK<VD>,
    public FactoryMixin< OdeSolverRK4<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverRK4() :
            OdeSolverExplicitRK<VD>(
                // https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
                typename ExplicitRKHelper<VD>::Coefficients({
                   // a
                   1./2,
                   0,    1./2,
                   0,    0,    1 }, {

                   // b
                   1./6, 1./3, 1./3, 1./6 }, {

                   // c
                   1./2, 1./2, 1 },
                   -1 ),
                   4 )
            {}
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESOLVERRK4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

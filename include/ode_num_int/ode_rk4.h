// ode_rk4.h

#ifndef _ODE_RK4_H_
#define _ODE_RK4_H_

#include "ode_explicit_rk.h"

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

#endif // _ODE_RK4_H_

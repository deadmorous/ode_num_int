// ode_explicit_euler.h

#ifndef _ODE_EXPLICIT_EULER_H_
#define _ODE_EXPLICIT_EULER_H_

#include "ode_explicit_rk.h"

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

#endif // _ODE_EXPLICIT_EULER_H_

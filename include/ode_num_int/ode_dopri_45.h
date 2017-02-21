// ode_dopri_45.h

#ifndef _ODE_DOPRI_45_H_
#define _ODE_DOPRI_45_H_

#include "ode_explicit_embedded_rk.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverDOPRI45 :
    public OdeSolverExplicitEmbeddedRK<VD>,
    public FactoryMixin< OdeSolverDOPRI45<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverDOPRI45() :
            OdeSolverExplicitEmbeddedRK<VD>(
                // https://en.wikipedia.org/wiki/Dormand%E2%80%93Prince_method
                typename ExplicitRKHelper<VD>::Coefficients({
                   // a
                   1./5,
                   3./40,        9./40,
                   44./45,      -56./15,      32./9,
                   19372./6561, -25360./2187, 64448./6561, -212./729,
                   9017./3168,  -355./33,     46732./5247,  49./176,  -5103./18656,
                   35./384,      0,           500./1113,    125./192, -2187./6784,    11./84 }, {

                   // bhi
                   35./384,      0,           500./1113,    125./192, -2187./6784,    11./84,    0,
                   // blo
                   5179./57600,  0,           7571./16695,  393./640, -92097./339200, 187./2100, 1./40 }, {

                   // c
                   1./5,         3./10,       4./5,         8./9,      1,              1 },
                   0 ),
                   5, 4 )
            {}
    };
//        void interp4( V& dst, real_type t ) const
//            {
//            ASSERT( dst.size() == m_k1.size() );
//            auto tt = t*t;
//            real_type b1 = t*(1. + t*((-1337./480) + t*((1039./360) + t*(-1163./1152)) ));
//            // real_type b2 = 0;
//            real_type b3 = tt*(100./3)*((1054./9275) + t*((-4682./27825) + t*(379./5565)));
//            real_type b4 = -2.5*((27./40) + t*((-9./5) + t*(83./96)));
//            real_type b5 = (18225./848)*tt*((-3./250) + t*((22./375) + t*(-37./600)));
//            real_type b6 = (-22./7)*tt*(-0.3 + t*((29./30) + t*(-17./24)));
//            auto n = m_k1.size();
//            auto pd = &dst[0];
//            auto pk1 = &m_k1[0];
//            auto pk3 = &m_k3[0];
//            auto pk4 = &m_k4[0];
//            auto pk5 = &m_k5[0];
//            auto pk6 = &m_k6[0];
//            for( decltype(n) i=0; i<n; ++i )
//                pd[i] = b1*pk1[i] + b3*pk3[i] + b4*pk4[i] + b5*pk5[i] + b6*pk6[i];
//            }

} // end namespace math
} // end namespace ctm

#endif // _ODE_DOPRI_45_H_

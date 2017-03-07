// OdeSolverDOPRI56.h

#ifndef _ODE_ODESOLVERDOPRI56_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODESOLVERDOPRI56_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeSolverExplicitEmbeddedRK.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverDOPRI56 :
    public OdeSolverExplicitEmbeddedRK<VD>,
    public FactoryMixin< OdeSolverDOPRI56<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverDOPRI56() :
            OdeSolverExplicitEmbeddedRK<VD>(
                // http://www.peterstone.name/Maplepgs/Maple/nmthds/RKcoeff/Runge_Kutta_schemes/RK6/RKcoeff6e_1.pdf
                typename ExplicitRKHelper<VD>::Coefficients({
                   // a
                   1./10,
                  -2./81,           20./81,
                   615./1372,      -270./343,    1053./1372,
                   3243./5500,     -54./55,      50949./71500,      4998./17875,
                  -26492./37125,    72./55,      2808./23375,      -24206./37125,       338./459,
                   5561./2376,     -35./11,     -24117./31603,      899983./200772,    -5225./1836,     3925./4056,
                   465467./266112, -2945./1232, -5610201./14158144, 10513573./3212352, -424325./205632, 376225./454272,  0 }, {

                   // bhi
                   61./864,         0,           98415./321776,     16807./146016,      1375./7344,     1375./5408,     -37./1120, 1./10,
                   // blo
                   821./10800,      0,           19683./71825,      175273./912600,     395./3672,      785./2704,       3./50,    0 }, {

                   // c
                   1./10,           2./9,        3./7,              3./5,               4./5,           1,               1 },
                   -1 ),
                6, 5 )
            {}
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_ODESOLVERDOPRI56_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

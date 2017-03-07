// ErrorInfNormCalculator.h

#ifndef _ODESOLVER_ERRORINFNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ERRORINFNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/ErrorNormCalculator.h"

namespace ctm {
namespace math {

template< class VD >
class ErrorInfNormCalculator :
    public ErrorNormCalculator<VD>,
    public FactoryMixin< ErrorInfNormCalculator<VD>, ErrorNormCalculator<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        real_type errorNorm( const V& error ) const
            {
            auto errorNorm = error.infNorm();
            this->errorNormCalculatorObservers( error, errorNorm );
            return errorNorm;
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ERRORINFNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

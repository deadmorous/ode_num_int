// ErrorNormCalculator.h

#ifndef _ODESOLVER_ERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeRhs.h"

namespace ctm {
namespace math {

template< class VD >
using ErrorNormCalculatorObservers = cxx::Observers<
    const VectorTemplate< VD >& /*error*/,
    const typename VD::value_type /*errorNorm*/ >;

template< class VD >
class ErrorNormCalculator :
    public Factory< ErrorNormCalculator<VD> >,
    public OdeRhsHolder<VD>,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        ErrorNormCalculatorObservers<VD> errorNormCalculatorObservers;

        virtual real_type errorNorm( const V& error ) const = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        ErrorNormCalculatorHolder, ErrorNormCalculator,
        errorNormCalculator, setErrorNormCalculator,
        onErrorNormCalculatorChanged, offErrorNormCalculatorChanged )

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ERRORNORMCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

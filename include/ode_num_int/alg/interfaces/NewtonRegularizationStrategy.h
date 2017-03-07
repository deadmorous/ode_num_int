// NewtonRegularizationStrategy.h

#ifndef _ALG_INTERFACES_NEWTONREGULARIZATIONSTRATEGY_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_NEWTONREGULARIZATIONSTRATEGY_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./VectorMappingRegularizer.h"
#include "./ErrorEstimator.h"
#include "./NewtonDescentDirection.h"

namespace ctm {
namespace math {

template< class VD >
class NewtonRegularizationStrategy :
    public VectorMappingRegularizerHolder,
    public ConstErrorEstimatorHolder<VD>,
    public NewtonDescentDirectionHolder<VD>,
    public Factory< NewtonRegularizationStrategy<VD> >,
    public OptionalParameters
    {
    public:
        virtual double regularizationParameter() const = 0;
        virtual void setRegularizationParameter( double param ) = 0;
        virtual bool adjustRegularizationParameter() = 0;
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonRegularizationStrategyHolder, NewtonRegularizationStrategy,
        regularizationStrategy, setRegularizationStrategy,
        onNewtonRegularizationStrategyChanged, offNewtonRegularizationStrategyChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_NEWTONREGULARIZATIONSTRATEGY_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

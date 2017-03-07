// NewtonIterationPerformer.h

#ifndef _ALG_INTERFACES_NEWTONITERATIONPERFORMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_NEWTONITERATIONPERFORMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./ErrorEstimator.h"
#include "./NewtonDescentDirection.h"
#include "./NewtonLinearSearch.h"

namespace ctm {
namespace math {

template< class VD >
class NewtonIterationPerformer :
    public MappingHolder< VD >,
    public ErrorEstimatorHolder< VD >,
    public NewtonDescentDirectionHolder< VD >,
    public NewtonLinearSearchHolder< VD >,
    public Factory< NewtonIterationPerformer<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        virtual void setInitialGuess( const V& initialGuess, bool hardReset = true ) = 0;
        virtual void reset( bool resetDescentDirection ) = 0;
        virtual typename ErrorEstimator<VD>::Status iteration( unsigned int iterationNumber ) = 0;
        virtual const V& currentSolution() const = 0;
        virtual const V& currentResidual() const = 0;
        virtual real_type currentStepRatio() const = 0;
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonIterationPerformerHolder, NewtonIterationPerformer,
        iterationPerformer, setIterationPerformer,
        onNewtonIterationPerformerChanged, offNewtonIterationPerformerChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_NEWTONITERATIONPERFORMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

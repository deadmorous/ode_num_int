// ExtrapolatorStepSequence.h

#ifndef _ODESOLVER_EXTRAPOLATORSTEPSEQUENCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_EXTRAPOLATORSTEPSEQUENCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/Factory.h"
#include "../../infra/def_prop_class.h"

namespace ctm {
namespace math {

class ExtrapolatorStepSequence :
    public Factory< ExtrapolatorStepSequence >
    {
    public:
        virtual void reset() = 0;
        virtual int next() = 0;
    };

CTM_DEF_PROP_CLASS(
        ExtrapolatorStepSequenceHolder,
        std::shared_ptr<ExtrapolatorStepSequence>, std::shared_ptr<ExtrapolatorStepSequence>,
        stepSequence, setStepSequence)

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_EXTRAPOLATORSTEPSEQUENCE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

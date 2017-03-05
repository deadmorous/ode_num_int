// ExtrapolatorStepSequenceHarmonic.h

#ifndef _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEHARMONIC_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEHARMONIC_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ExtrapolatorStepSequence.h"

namespace ctm {
namespace math {

class ExtrapolatorStepSequenceHarmonic :
    public ExtrapolatorStepSequence,
    public FactoryMixin< ExtrapolatorStepSequenceHarmonic, ExtrapolatorStepSequence >
    {
    public:
        ExtrapolatorStepSequenceHarmonic() {
            reset();
            }

        void reset() {
            m_current = 1;
            }

        int next() {
            return m_current++;
            }
    private:
        int m_current;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEHARMONIC_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

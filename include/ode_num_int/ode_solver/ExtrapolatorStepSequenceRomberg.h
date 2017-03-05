// ExtrapolatorStepSequenceRomberg.h

#ifndef _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEROMBERG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEROMBERG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ExtrapolatorStepSequence.h"

namespace ctm {
namespace math {

class ExtrapolatorStepSequenceRomberg :
    public ExtrapolatorStepSequence,
    public FactoryMixin< ExtrapolatorStepSequenceRomberg, ExtrapolatorStepSequence >
    {
    public:
        ExtrapolatorStepSequenceRomberg() {
            reset();
            }

        void reset() {
            m_current = 1;
            }

        int next()
            {
            int result = m_current;
            m_current <<= 1;
            return result;
            }
    private:
        int m_current;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEROMBERG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

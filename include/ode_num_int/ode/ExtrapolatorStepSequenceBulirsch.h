// ExtrapolatorStepSequenceBulirsch.h

#ifndef _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEBULIRSCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEBULIRSCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/ExtrapolatorStepSequence.h"

namespace ctm {
namespace math {

class ExtrapolatorStepSequenceBulirsch :
    public ExtrapolatorStepSequence,
    public FactoryMixin< ExtrapolatorStepSequenceBulirsch, ExtrapolatorStepSequence >
    {
    public:
        ExtrapolatorStepSequenceBulirsch() {
            reset();
            }

        void reset()
            {
            m_ordinal = 0;
            m_currentOdd = 1;
            m_currentEven = 2;
            }

        int next()
            {
            ++m_ordinal;
            if( m_ordinal & 1 )
                return m_currentOdd;
            else {
                int result = m_currentEven;
                if( m_ordinal == 2 )
                    m_currentOdd = 3;
                else
                    m_currentOdd <<= 1;
                m_currentEven <<= 1;
                return result;
                }
            }
    private:
        int m_ordinal;
        int m_currentOdd;
        int m_currentEven;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_EXTRAPOLATORSTEPSEQUENCEBULIRSCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

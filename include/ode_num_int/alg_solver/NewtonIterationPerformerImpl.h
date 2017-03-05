// NewtonIterationPerformerImpl.h

#ifndef _ALGSOLVER_NEWTONITERATIONPERFORMERIMPL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_NEWTONITERATIONPERFORMERIMPL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "NewtonIterationPerformer.h"

namespace ctm {
namespace math {

template< class VD >
class NewtonIterationPerformerImpl :
    public NewtonIterationPerformer< VD >,
    public FactoryMixin< NewtonIterationPerformerImpl<VD>, NewtonIterationPerformer<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        NewtonIterationPerformerImpl() :
            m_f_valid( false ),
            m_currentStepRatio( 1 ),
            m_hardResetTried( false )
            {}

        void setInitialGuess( const V& initialGuess, bool hardReset = true )
            {
            m_x = initialGuess;
            m_f_valid = false;
            reset( hardReset );
            }

        void reset( bool hardResetDescentDirection )
            {
            m_currentStepRatio = 1;
            this->errorEstimator()->reset( m_x );
            this->newtonDescentDirection()->reset( hardResetDescentDirection );
            }

        typename ErrorEstimator<VD>::Status iteration( unsigned int iterationNumber )
            {
            if( iterationNumber == 0 )
                m_hardResetTried = false;
            auto f0 = currentResidual();
            auto ddir = this->newtonDescentDirection();
            auto dir = ddir->computeDescentDirection( m_x, f0, iterationNumber );
            m_currentStepRatio = this->newtonLinearSearch()->lineSearch( dir, m_x, f0, &m_f );
            m_x += dir.scaled( m_currentStepRatio );
            this->errorEstimator()->setCurrentSolution( m_x, m_f );
            auto result = this->errorEstimator()->currentStatus();
            if( m_currentStepRatio == 0   &&   result == ErrorEstimator<VD>::ContinueIterations )
                result = ErrorEstimator<VD>::Diverged;
            return result;
            }

        const V& currentSolution() const {
            return m_x;
            }

        const V& currentResidual() const
            {
            if( !m_f_valid ) {
                m_f = this->mapping()->map( m_x );
                m_f_valid = true;
                }
            return m_f;
            }

        real_type currentStepRatio() const {
            return m_currentStepRatio;
            }

    private:
        V m_x;
        mutable V m_f;
        mutable bool m_f_valid;
        real_type m_currentStepRatio;
        bool m_hardResetTried;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_NEWTONITERATIONPERFORMERIMPL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

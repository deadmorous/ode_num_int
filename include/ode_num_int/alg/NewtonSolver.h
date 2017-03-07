// NewtonSolver.h

#ifndef _ALGSOLVER_NEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_NEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/NewtonSolverInterface.h"
#include "NewtonIterationPerformerImpl.h"
#include "SimpleErrorEstimator.h"
#include "SimpleNewtonDescentDirection.h"
#include "SimpleNewtonLinearSearch.h"
#include "DefaultJacobianProvider.h"

namespace ctm {
namespace math {

template< class VD >
class NewtonSolver :
    public NewtonSolverInterface<VD>,
    public FactoryMixin< NewtonSolver<VD>, NewtonSolverInterface<VD> >
    {
    public:
        typedef NewtonSolverInterface<VD> BaseClass;
        typedef typename BaseClass::Status Status;
        typedef VectorTemplate< VD > V;

        NewtonSolver() :
            m_iterationCountLimit(DefaultIterationCountLimit)
            {
            this->setComponent( std::make_shared< NewtonIterationPerformerImpl<VD> >() );
            this->setComponent( std::make_shared< SimpleErrorEstimator<VD> >() );
            this->setComponent( std::make_shared< SimpleNewtonDescentDirection<VD> >() );
            this->setComponent( std::make_shared< SimpleNewtonLinearSearch<VD> >() );
            this->setComponent( std::make_shared< DefaultJacobianProvider<VD> >() );
            }

        unsigned int iterationCountLimit() const {
            return m_iterationCountLimit;
            }

        void setIterationCountLimit( unsigned int iterationCountLimit ) {
            m_iterationCountLimit = iterationCountLimit;
            }

        void setInitialGuess( const V& initialGuess, bool hardReset = true ) {
            this->iterationPerformer()->setInitialGuess( initialGuess, hardReset );
            }

        Status run()
            {
            auto itperf = this->iterationPerformer();
            auto ddir = itperf->newtonDescentDirection();
            itperf->reset( false );
            auto hardResetTried = false;
            auto x0 = currentSolution();
            auto iterationNumberBias = 0u;
            while( true ) {
                auto breakLoop = false;
                auto iterationNumber=0u;
                for( ; !breakLoop && iterationNumber<m_iterationCountLimit; ++iterationNumber ) {
                    auto status = itperf->iteration( iterationNumber );
                    if( this->iterationObservers( iterationNumber + iterationNumberBias, this ) )
                        return BaseClass::Terminated;
                    switch( status ) {
                        case ErrorEstimator<VD>::Converged:
                            if( maybeAdjustRegularizationParameter() )
                                break;
                            ddir->reportSuccess( true );
                            return BaseClass::Converged;
                        case ErrorEstimator<VD>::ContinueIterations:
                            maybeAdjustRegularizationParameter();
                            break;
                        case ErrorEstimator<VD>::Diverged:
                            if( maybeAdjustRegularizationParameter() )
                                break;
                            ddir->reportSuccess( false );
                            if( !hardResetTried ) {
                                breakLoop = true;
                                break;
                                }
                            return BaseClass::Diverged;
                        default:
                            ASSERT( false );
                        }
                    }
                if( hardResetTried )
                    break;
                else if( ddir->hardResetMayHelp() ) {
                    setInitialGuess( x0, true );
                    hardResetTried = true;
                    iterationNumberBias += iterationNumber;
                    }
                else
                    break;
                }
            ddir->reportSuccess( false );
            return BaseClass::IterationCountLimitExceeded;
            }

        const V& currentSolution() const {
            return this->iterationPerformer()->currentSolution();
            }

    private:
        static const unsigned int DefaultIterationCountLimit = 100;
        unsigned int m_iterationCountLimit;
        bool maybeAdjustRegularizationParameter()
            {
            auto rs = this->regularizationStrategy();
            return rs ?   rs->adjustRegularizationParameter() :   false;
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_NEWTONSOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

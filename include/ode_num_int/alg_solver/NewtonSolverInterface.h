// NewtonSolverInterface.h

#ifndef _ALGSOLVER_NEWTONSOLVERINTERFACE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_NEWTONSOLVERINTERFACE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "NewtonIterationPerformer.h"
#include "NewtonRegularizationStrategy.h"

namespace ctm {
namespace math {

template< class VD > class NewtonSolverInterface;

template< class VD >
using NewtonSolverIterationObservers = cxx::TerminatingObservers<
        unsigned int /*iterationNumber*/,
        const NewtonSolverInterface<VD>* /*solver*/>;



template< class VD >
class NewtonSolverInterface :
    public NewtonIterationPerformerHolder<VD>,
    public NewtonRegularizationStrategyHolder<VD>,
    public OptionalParameters,
    public Factory< NewtonSolverInterface<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef OptionalParameters::Parameters Parameters;

        NewtonSolverIterationObservers<VD> iterationObservers;

        enum Status {
            Converged,
            Diverged,
            Terminated,
            IterationCountLimitExceeded
        };

        virtual unsigned int iterationCountLimit() const = 0;
        virtual void setIterationCountLimit( unsigned int iterationCountLimit ) = 0;
        virtual void setInitialGuess( const V& initialGuess, bool hardReset = true ) = 0;
        virtual Status run() = 0;
        virtual const V& currentSolution() const = 0;

        static std::string statusText( Status status )
            {
            // Note: intentionally not static, to avoid the non-unloadable module problem
            struct { Status status; const char *text; } d[] = {
                { Converged, "converged" },
                { Diverged, "diverged" },
                { Terminated, "terminated" },
                { IterationCountLimitExceeded, "iteration count limit exceeded" }
                };
            for( const auto& x : d )
                if( x.status == status )
                    return x.text;
            ASSERT( false );
            return "unknown";
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonIterationPerformer<VD> >& itperf )
            {
            this->setIterationPerformer( itperf );
            auto rs = this->regularizationStrategy();
            if( rs ) {
                rs->setErrorEstimator( itperf->errorEstimator() );
                rs->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< VectorMapping<VD> >& eqnRhs )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                itperf->setMapping( eqnRhs );
                itperf->newtonDescentDirection()->setMapping( eqnRhs );
                itperf->newtonLinearSearch()->setMapping( eqnRhs );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< ErrorEstimator<VD> >& errorEstimator )
            {
            auto itperf = this->iterationPerformer();
            if( itperf )
                itperf->setErrorEstimator( errorEstimator );
            auto rs = this->regularizationStrategy();
            if( rs )
                rs->setErrorEstimator( errorEstimator );
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonDescentDirection<VD> >& ddir )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                ddir->setMapping( itperf->mapping() );
                itperf->setNewtonDescentDirection( ddir );
                }
            auto rs = this->regularizationStrategy();
            if( rs )
                rs->setNewtonDescentDirection( ddir );
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< JacobianProvider<VD> >& jacobianProvider )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                if( auto ddir = itperf->newtonDescentDirection() )
                    ddir->setJacobianProvider( jacobianProvider );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonLinearSearch<VD> >& lineSearch )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                lineSearch->setMapping( itperf->mapping() );
                itperf->setNewtonLinearSearch( lineSearch );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonRegularizationStrategy<VD> >& regularizationStrategy )
            {
            this->setRegularizationStrategy( regularizationStrategy );
            if( regularizationStrategy ) {
                auto itperf = this->iterationPerformer();
                if( itperf ) {
                    regularizationStrategy->setErrorEstimator( itperf->errorEstimator() );
                    regularizationStrategy->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                    }
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< JacobianTrimmer<VD> >& jacobianTrimmer )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                if( auto ddir = itperf->newtonDescentDirection() ) {
                    if( auto jp = ddir->jacobianProvider() )
                        jp->setJacobianTrimmer( jacobianTrimmer );
                    }
                }
            return *this;
            }

        Parameters parameters() const
            {
            Parameters result;
            auto itperf = this->iterationPerformer();
            result["itperf"] = itperf;
            result["regularizer"] = this->regularizationStrategy();
            result["max_iterations"] = this->iterationCountLimit();
            if( itperf ) {
                auto ddir = itperf->newtonDescentDirection();
                result["ddir"] = ddir;
                if( ddir ) {
                    result["jacobian"] = ddir->jacobianProvider();
                    result["jtrim"] = ddir->jacobianProvider()->jacobianTrimmer();
                    }
                result["linsearch"] = itperf->newtonLinearSearch();
                result["err"] = itperf->errorEstimator();
                }
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OptionalParameters::maybeSetParameter(
                        parameters, "itperf",
                        std::bind( &NewtonSolverInterface<VD>::setIterationPerformer, this, std::placeholders::_1 ) );
            auto itperf = this->iterationPerformer();
            OptionalParameters::maybeSetParameter(
                        parameters, "regularizer",
                        [this, itperf]( const std::shared_ptr< NewtonRegularizationStrategy<VD> >& rs )
                        {
                        this->setRegularizationStrategy( rs );
                        if( rs && itperf ) {
                            rs->setErrorEstimator( itperf->errorEstimator() );
                            rs->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                            }
                        } );
            OptionalParameters::maybeSetParameter(
                        parameters, "max_iterations",
                        std::bind( &NewtonSolverInterface<VD>::setIterationCountLimit, this, std::placeholders::_1 ) );
            if( itperf ) {
                OptionalParameters::maybeSetParameter(
                            parameters, "ddir",
                            [this, itperf]( const std::shared_ptr< NewtonDescentDirection<VD> >& ddir )
                            {
                            if( ddir )
                                ddir->setMapping( itperf->mapping() );
                            itperf->setNewtonDescentDirection( ddir );
                            auto rs = this->regularizationStrategy();
                            if( rs )
                                rs->setNewtonDescentDirection( ddir );
                            } );
                auto ddir = itperf->newtonDescentDirection();
                if( ddir ) {
                    OptionalParameters::maybeSetParameter(
                                parameters, "jacobian",
                                std::bind( &NewtonDescentDirection<VD>::setJacobianProvider, ddir.get(), std::placeholders::_1 ) );
                    auto jp = ddir->jacobianProvider();
                    if( jp )
                        OptionalParameters::maybeSetParameter(
                                    parameters, "jtrim",
                                    std::bind( &JacobianProvider<VD>::setJacobianTrimmer, jp.get(), std::placeholders::_1 ) );
                    }

                OptionalParameters::maybeSetParameter(
                            parameters, "linsearch",
                            [itperf]( const std::shared_ptr< NewtonLinearSearch<VD> >& lineSearch )
                            {
                            lineSearch->setMapping( itperf->mapping() );
                            itperf->setNewtonLinearSearch( lineSearch );
                            } );

                OptionalParameters::maybeSetParameter(
                            parameters, "err",
                            [this, itperf]( const std::shared_ptr< ErrorEstimator<VD> >& errorEstimator )
                            {
                            itperf->setErrorEstimator( errorEstimator );
                            auto rs = this->regularizationStrategy();
                            if( rs )
                                rs->setErrorEstimator( errorEstimator );
                            } );
                }
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["itperf"] = OptionalParameters::appendNestedHelp(
                        this->iterationPerformer(),
                        "Iteration performer" );
            result["regularizer"] = OptionalParameters::appendNestedHelp(
                        this->regularizationStrategy(),
                        "Regularization strategy" );
            result["max_iterations"] = "Iteration count limit (if exceeded, exiting with the 'diverged' status)";

            if( auto itperf = this->iterationPerformer() ) {
                result["ddir"] = OptionalParameters::appendNestedHelp(
                            itperf->newtonDescentDirection(),
                            "Descent direction algorithm" );
                if( auto ddir = itperf->newtonDescentDirection() ) {
                    result["jacobian"] = OptionalParameters::appendNestedHelp(
                                ddir->jacobianProvider(),
                                "Jacobian provider" );
                    if( auto jp = ddir->jacobianProvider() )
                        result["jtrim"] = OptionalParameters::appendNestedHelp(
                                    jp->jacobianTrimmer(),
                                    "Jacobian trimmer" );
                    }
                result["linsearch"] = OptionalParameters::appendNestedHelp(
                            itperf->newtonLinearSearch(),
                            "Line search algorithm" );
                result["err"] = OptionalParameters::appendNestedHelp(
                            itperf->errorEstimator(),
                            "Error estimator" );
                }

            return result;
            }
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonSolverHolder, NewtonSolverInterface,
        newtonSolver, setNewtonSolver,
        onNewtonSolverChanged, offNewtonSolverChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_NEWTONSOLVERINTERFACE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

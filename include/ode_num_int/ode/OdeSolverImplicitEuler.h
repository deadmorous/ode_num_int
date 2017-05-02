// OdeSolverImplicitEuler.h

#ifndef _ODE_ODESOLVERIMPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODESOLVERIMPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeNewtonSolver.h"
#include "./interfaces/OdeEventController.h"
#include "./OdeStepMappingEuler.h"
#include "../alg/VectorReorderingMapping.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverImplicitEuler :
    public OdeSolver<VD>,
    public OdeNewtonSolver<VD>,
    public OdeSolverEventController<VD>,
    public FactoryMixin< OdeSolverImplicitEuler<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverImplicitEuler() :
            OdeNewtonSolver<VD>( *this, "newton" ),
            OdeSolverEventController<VD>( *this, "linear" ),
            m_predictorSteps( 1 ),
            m_mapping4Newton( std::make_shared< OdeStepMappingEuler<VD> >() )
            {
            auto setupNewtonSolver = [this]() {
                auto newton = this->newtonSolver();
                // Set vector mapping for the Newton's solver
                if( m_reorder )
                    this->newtonSolver()->setComponent( m_reorder );
                else
                    this->newtonSolver()->setComponent( m_mapping4Newton );
               cxx::forwardObservers( newton->iterationObservers, this->iterationObservers );
                if( auto itperf = newton->iterationPerformer() ) {
                    // Forward jacobian refresh observers
                    auto setupDdir = [this, itperf] {
                        if( auto ddir = itperf->newtonDescentDirection() )
                            cxx::forwardObservers( ddir->jacobianRefreshObservers, this->jacobianRefreshObservers );
                        };
                    itperf->onNewtonDescentDirectionChanged( setupDdir );
                    setupDdir();
                    }
                };
            setupNewtonSolver();
            this->onNewtonSolverChanged( setupNewtonSolver );

            this->onOdeRhsChanged([this](){
                m_mapping4Newton->setOdeRhs( this->odeRhs() );
                });
            }

        int order() const {
            return 1;
            }

        real_type initialTime() const {
            return m_mapping4Newton->initialTime();
            }

        V initialState() const {
            return m_mapping4Newton->initialState();
            }

        void setInitialState( real_type initialTime, const V& initialState, bool /*soft*/ = false ) {
            m_mapping4Newton->setInitialState( initialTime, initialState );
            }

        void doStep()
            {
            auto h = this->initialStepSize();
            this->odeSolverPreObservers( h, this );

            OpaqueTickCounter timer;    // deBUG
            timer.Start();

            auto initialTime = m_mapping4Newton->initialTime();
            auto& initialState = m_mapping4Newton->initialState();

            auto newton = this->newtonSolver();
            m_mapping4Newton->setTimeStep( h );

            if( m_predictor ) {
                m_predictor->setOdeRhs( this->odeRhs() );
                m_predictor->setInitialStepSize( h/m_predictorSteps );
                m_predictor->setInitialState( initialTime, initialState );
                for( auto istep=0u; istep<m_predictorSteps; ++istep )
                    m_predictor->doStep();
                m_mapping4Newton->computeEqnState( m_eqnbuf, m_predictor->initialState() );
                if( m_reorder )
                    m_eqnbuf = m_reorder->orderInput( m_eqnbuf );
                newton->setInitialGuess( m_eqnbuf, false );
                }
            else if( m_reorder ) {
                m_reorder->orderInput( m_eqnbuf, m_mapping4Newton->eqnInitialState() );
                newton->setInitialGuess( m_eqnbuf, false );
                }
            else
                newton->setInitialGuess( m_mapping4Newton->eqnInitialState(), false );

            this->tstat0.add( timer.Lap() );
            m_mapping4Newton->beforeStep();

            auto ec = this->eventController();
            if( ec )
                ec->atStepStart( initialState, initialTime );

            auto status = newton->run();
            if( status != NewtonSolverInterface<VD>::Converged )
                throw cxx::exception(std::string("Algebraic solver failed: ") + newton->statusText( status ) );
            if( m_reorder ) {
                m_reorder->unorderInput( m_eqnbuf, newton->currentSolution() );
                m_mapping4Newton->computeOdeState( m_buf, m_eqnbuf );
                }
            else
                m_mapping4Newton->computeOdeState( m_buf, newton->currentSolution() );

            this->tstat1.add( timer.Lap() );

            // Process events
            auto nextTime = initialTime + h;
            bool stepTruncated = false;
            unsigned int izfTrunc = ~0u;
            int transitionType = 0;
            if( ec ) {
                auto interpolate = linearInterpolator( initialState, m_buf );
                stepTruncated = ec->atStepEnd( initialState, m_buf, nextTime, interpolate, &izfTrunc, &transitionType );
                if( stepTruncated )
                    newton->iterationPerformer()->newtonDescentDirection()->reset( true );
                }
            m_mapping4Newton->setInitialState( nextTime, m_buf );
            this->odeSolverPostObservers(
                OdeSolverPostObserverArg<VD>()
                    .setStepSize( h )
                    .setStepTruncated( stepTruncated )
                    .setIzfTrunc( izfTrunc )
                    .setTransitionType( transitionType )
                    .setSolver( this ) );

            // Add LU timing stats from Newton's method
            auto& newtonLu = newton->iterationPerformer()->newtonDescentDirection()->luTimingStats;
            this->luTimingStats += newtonLu;
            newtonLu = typename LUFactorizer<real_type>::TimingStats();

            this->tstat2.add( timer.Lap() );
            }

        std::shared_ptr< VectorReorderingMapping<VD> > reorder() const {
            return m_reorder;
            }

        void setReorder( const std::shared_ptr< VectorReorderingMapping<VD> >& reorder ) {
            m_reorder = reorder;
            if( m_reorder )
                m_reorder->setMapping( m_mapping4Newton );
            if( m_reorder )
                this->newtonSolver()->setComponent( m_reorder );
            else
                this->newtonSolver()->setComponent( m_mapping4Newton );
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolver<VD>::parameters();
            result["alpha"] = m_mapping4Newton->implicitnessParameter();
            result["predictor"] = m_predictor;
            result["predictor_steps"] = m_predictorSteps;
            result["reorder"] = m_reorder;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            OptionalParameters::maybeSetParameter(
                        parameters, "alpha",
                        std::bind( &OdeStepMappingEuler<VD>::setImplicitnessParameter, m_mapping4Newton.get(), std::placeholders::_1 ) );
            OptionalParameters::maybeLoadParameter( parameters, "predictor", m_predictor );
            OptionalParameters::maybeLoadParameter( parameters, "predictor_steps", m_predictorSteps );
            OptionalParameters::maybeSetParameter(
                        parameters, "reorder",
                        std::bind( &OdeSolverImplicitEuler<VD>::setReorder, this, std::placeholders::_1 ) );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolver<VD>::helpOnParameters();
            result["alpha"] = "Implicitness parameter: 0 gives explicit Euler, 0.5 - trapezoidal rule, 1 - implicit Euler";
            result["predictor"] = OptionalParameters::appendNestedHelp(
                        m_predictor, "ODE solver to use as a predictor for the initial guess" );
            result["predictor_steps"] = "Number of predictor steps per solver step";
            result["reorder"] = OptionalParameters::appendNestedHelp(
                        m_reorder, "Variable reordering (might be important for performance and Jacobian trimming)" );
            return result;
            }

        std::string helpOnType() const {
            return "Implicit alpha method, e.g. implicit Euler at alpha=1 and trapezoidal rule at alpha=0.5";
            }

    private:
        std::shared_ptr< OdeSolver<VD> > m_predictor;
        unsigned int m_predictorSteps;
        std::shared_ptr< VectorReorderingMapping<VD> > m_reorder;

        std::shared_ptr< OdeStepMappingEuler<VD> > m_mapping4Newton;
        V m_buf;
        V m_eqnbuf;
    };


} // end namespace math
} // end namespace ctm

#endif // _ODE_ODESOLVERIMPLICITEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

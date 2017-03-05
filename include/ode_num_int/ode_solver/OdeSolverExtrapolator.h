// OdeSolverExtrapolator.h

#ifndef _ODESOLVER_ODESOLVEREXTRAPOLATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVEREXTRAPOLATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolver.h"
#include "ExtrapolatorStepSequence.h"
#include "OdeSolverErrorNormCalculator.h"
#include "OdeSolverStepSizeController.h"
#include "OdeSolverEventController.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverExtrapolator :
    public OdeSolver<VD>,
    public ExtrapolatorStepSequenceHolder,
    public OdeSolverErrorNormCalculator<VD>,
    public OdeSolverStepSizeController<VD>,
    public OdeSolverEventController<VD>,
    public FactoryMixin< OdeSolverExtrapolator<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverExtrapolator() :
            OdeSolverStepSizeController<VD>( *this, "simple" ),
            OdeSolverEventController<VD>( *this, "linear" ),
            OdeSolverErrorNormCalculator<VD>( *this, "inf" ),
            m_symmetric( false ),
            m_disable_error_check( false ),
            m_max_stages( 4 )
            {
            this->onOdeRhsChanged( getSyncOdeRhs() );
            }

        int order() const
            {
            if( auto rs = referenceSolver() ) {
                auto rsOrder = rs->order();
                return rsOrder + ( m_max_stages - 1 )*( m_symmetric? 2: 1 );
                }
            else
                return -1;  // TODO
            }

        int lowOrder() const {
            return order() - ( m_symmetric? 2: 1 );
            }

        unsigned int maxStages() const {
            return m_max_stages;
            }

        void setMaxStages( unsigned int maxStages ) {
            m_max_stages = maxStages;
            }

        std::shared_ptr< OdeSolver<VD> > referenceSolver() const {
            return m_referenceSolver;
            }

        void setReferenceSolver( const std::shared_ptr< OdeSolver<VD> >& referenceSolver )
            {
            m_referenceSolver = referenceSolver;
            syncOdeRhs();
            }

        real_type initialTime() const {
            return m_initialTime;
            }

        V initialState() const {
            return m_initialState;
            }

        void setInitialState( real_type initialTime, const V& initialState, bool soft = false )
            {
            m_initialTime = initialTime;
            m_initialState = initialState;
            if( !soft )
                m_referenceSolver->setInitialState( initialTime, initialState, false );
            cleanupStep();
            }

        void doStep()
            {
            while( true ) {
                this->odeSolverPreObservers( m_h, this );

                OpaqueTickCounter timer;    // deBUG

                timer.Start();
                initStep();

                auto rhs = this->odeRhs().get();

                this->tstat0.add( timer.Lap() );
                rhs->beforeStep( m_initialTime, m_initialState );

                auto ec = this->eventController();
                if( ec )
                    ec->atStepStart( m_initialState, m_initialTime );

                auto ss = stepSequence().get();
                ASSERT( ss );
                ss->reset();
                auto solver = m_referenceSolver.get();
                ASSERT( solver );
                for( unsigned int k=0; k<m_max_stages; ++k ) {
                    solver->setInitialState( m_initialTime, m_initialState, true );
                    auto n = ss->next();
                    auto h = m_h / n;
                    solver->setInitialStepSize( h );
                    for( int istep=0; istep<n; ++istep )
                        solver->doStep();
                    m_T[k] = solver->initialState();
                    m_n[k] = n;
                    }

                // Hairer I, p. 237: Aitken - Neville algorithm
                // See also
                // https://en.wikipedia.org/wiki/Neville's_algorithm
                //
                //                         T(j,k) - T(j-1,k)
                // T(j,k+1) = T(j,k)   +   -----------------
                //                               r - 1
                //
                // where r = (n(j)/n(j-k))^2 for symmetric solvers, and
                //       r = (n(j)/n(j-k)) for nonsymmetric solvers.
                for( unsigned int k=1; k<m_max_stages; ++k ) {
                    for( unsigned int j=k+1; j<=m_max_stages; ++j ) {
                        auto& T1 = m_T[j-k-1];  // T(j-1,k) -> T(j,k+1)
                        auto& T2 = m_T[j-k];    // T(j,k)
                        real_type r = static_cast<real_type>(m_n[j-1])/m_n[j-k-1];
                        if( m_symmetric )
                            r *= r;
                        auto d = 1. / ( r - 1.0 );
                        T1 *= -d;
                        T1 += T2.scaled( 1 + d );
                        }
                }
                this->tstat1.add( timer.Lap() );

                // The extrapolated solution of order m_max_stages is now in m_T[0],
                // and of order m_max_stages-1 in m_T[1]

                // Solution at the end of the step
                auto& nextState = m_T[0];

                // Compute error norm
                real_type errorNorm = 0;
                if( !m_disable_error_check ) {
                    auto& err = m_T[1];
                    err *= -1;
                    err += nextState;
                    errorNorm = this->errorNormCalculator()->errorNorm( err );
                    }

                // Compute step size, if error check is enabled
                real_type nextTime = m_initialTime + m_h;
                auto hcur = m_h;
                auto scResult = m_disable_error_check ?
                            typename OdeStepSizeController<VD>::Result( m_h, true, false ) :
                            this->odeStepSizeController()->controlStepSize( m_h, errorNorm, lowOrder() );

                this->tstat2.add( timer.Lap() );

                // Process events, if step is accepted
                bool stepTruncated = false;
                unsigned int izfTrunc = ~0u;
                int transitionType = 0;

                if( scResult.acceptStep ) {
                    if( ec ) {
                        auto interpolate = linearInterpolator( m_initialState, nextState );
                        stepTruncated = ec->atStepEnd( m_initialState, nextState, nextTime, interpolate, &izfTrunc, &transitionType );
                        }
                    m_initialTime = nextTime;
                    m_initialState.swap( nextState );
                    }

                // Apply new step size, if it has changed
                if( scResult.changeStepSize ) {
                    m_h = scResult.newStepSize;
                    if( m_h <= this->stepSizeMinThreshold() )
                        throw cxx::exception( "Step size has decreased to the minimum or below" );
                    }

                this->tstat3.add( timer.Lap() );

                // Invoke observers
                this->odeSolverPostObservers( m_h, scResult.acceptStep, scResult.changeStepSize, stepTruncated, errorNorm, izfTrunc, transitionType, this );

                this->tstat4.add( timer.Lap() );

                // Stop the loop if the step is accepted
                if( scResult.acceptStep )
                    break;
                }
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolver<VD>::parameters();
            result["symmetric"] = m_symmetric;
            result["disable_error_check"] = m_disable_error_check;
            result["max_stages"] = m_max_stages;
            result["step_sequence"] = this->stepSequence();
            result["reference_solver"] = m_referenceSolver;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            this->maybeLoadParameter( parameters, "symmetric", m_symmetric );
            this->maybeLoadParameter( parameters, "disable_error_check", m_disable_error_check );
            this->maybeLoadParameter( parameters, "max_stages", m_max_stages );
            this->maybeSetParameter(
                        parameters, "step_sequence",
                        std::bind( &OdeSolverExtrapolator<VD>::setStepSequence, this, std::placeholders::_1 ) );
            this->maybeLoadParameter( parameters, "reference_solver", m_referenceSolver );

            syncOdeRhs();
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["disable_error_check"] = "Disable error check and always use the initial step size.";
            result["step_sequence"] = this->appendNestedHelp( stepSequence(), "The type of step size sequence for the extrapolation." );
            result["reference_solver"] = this->appendNestedHelp( m_referenceSolver, "Reference solver to use." );
            return result;
            }

    private:
        std::shared_ptr< OdeSolver<VD> > m_referenceSolver;
        bool m_symmetric;
        bool m_disable_error_check;
        unsigned int m_max_stages;

        real_type m_initialTime;        // Initial time
        V m_initialState;               // Initial state

        real_type m_h;                  // Current step size

        typedef std::vector<V> VV;
        VV m_T;
        V m_buf;
        std::vector<unsigned int> m_n;

        void initStep()
            {
            if( !m_T.empty() )
                return;

            if( !m_referenceSolver )
                throw cxx::exception( "Reference solver is not set" );
            if( !stepSequence() )
                throw cxx::exception( "Step sequence is not set" );

            auto rhs = this->odeRhs();
            auto n = rhs->varCount();

            // Should already have an initial state
            ASSERT( m_initialState.size() == n );

            // Allocate memory
            m_h = this->initialStepSize();
            m_T = VV( computeTsize(), V(n) );
            m_n.resize( m_T.size() );
            }

        void cleanupStep()
            {
            if( auto ec = this->eventController() )
                ec->reset();
            m_T.clear();
            }

        unsigned int computeTsize() const
            {
            auto ss = stepSequence();
            ASSERT( ss );
            ss->reset();
            for( unsigned int i=0; i+1<m_max_stages; ++i )
                ss->next();
            return ss->next();
            }

        void syncOdeRhs()
            {
            if( m_referenceSolver )
                m_referenceSolver->setOdeRhs( this->odeRhs() );
            }

        std::function<void()> getSyncOdeRhs() {
            return std::bind( &OdeSolverExtrapolator::syncOdeRhs, this );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESOLVEREXTRAPOLATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

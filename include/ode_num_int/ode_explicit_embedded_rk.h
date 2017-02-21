// ode_explicit_embedded_rk.h

#ifndef _ODE_EXPLICIT_EMBEDDED_RK_H_
#define _ODE_EXPLICIT_EMBEDDED_RK_H_

#include "ode_solver.h"
#include "algebraic_eqn_solver.h"
#include "ode_event_controller.h"
#include "ode_explicit_rk_helper.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverExplicitEmbeddedRK :
    public OdeSolver<VD>,
    public OdeSolverErrorNormCalculator<VD>,
    public OdeSolverStepSizeController<VD>,
    public OdeSolverEventController<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        enum Mode {
            HighMode = 0,
            LowMode  = 1
            };

        OdeSolverExplicitEmbeddedRK(
                const typename ExplicitRKHelper<VD>::Coefficients& coefficients,
                unsigned int highOrder,
                unsigned int lowOrder ) :
            OdeSolverErrorNormCalculator<VD>( *this, "inf" ),
            OdeSolverStepSizeController<VD>( *this, "simple" ),
            OdeSolverEventController<VD>( *this, "linear" ),
            m_coefficients( coefficients ),
            m_highOrder( highOrder ),
            m_lowOrder( lowOrder ),
            m_initialTime( 0 ),
            m_disable_error_check( false ),
            m_force_error_calc( false ),
            m_h_after_switch( 0 ),
            m_mode( LowMode )
            {}

        int order() const {
            return m_mode == HighMode ?   m_highOrder :   m_lowOrder;
            }

        Mode mode() const {
            return m_mode;
            }

        void setMode( Mode mode ) {
            m_mode = mode;
            }

        real_type initialTime() const {
            return m_initialTime;
            }

        V initialState() const {
            return m_initialState;
            }

        void setInitialState( real_type initialTime, const V& initialState, bool /*soft*/ = false )
            {
            m_initialTime = initialTime;
            m_initialState = initialState;
            cleanupStep();
            }

        void doStep()
            {
            while( true ) {
                this->odeSolverPreObservers( m_h, this );

                OpaqueTickCounter timer;    // deBUG

                timer.Start();
                initStep();

                auto rhs = this->odeRhs();

                this->tstat0.add( timer.Lap() );
                rhs->beforeStep( m_initialTime, m_initialState );

                auto ec = this->eventController();
                if( ec )
                    ec->atStepStart( m_initialState, m_initialTime );

                // Compute solution at the end of the step
                RKHelper::compute( m_output, m_coefficients, rhs.get(), m_initialState, m_initialTime, m_h );
                this->tstat1.add( timer.Lap() );
                V& nextState = m_output.x2[m_mode];

                // Compute error norm
                real_type errorNorm = 0;
                if( !m_disable_error_check   ||   m_force_error_calc ) {
                    RKHelper::copy( m_output.buf, m_output.x2[HighMode] );
                    m_output.buf -= m_output.x2[LowMode];
                    errorNorm = this->errorNormCalculator()->errorNorm( m_output.buf );
                    }

                // Compute step size, if error check is enabled
                real_type nextTime = m_initialTime + m_h;
                auto hcur = m_h;
                auto scResult = m_disable_error_check ?
                            typename OdeStepSizeController<VD>::Result( m_h, true, false ) :
                            this->odeStepSizeController()->controlStepSize( m_h, errorNorm, m_lowOrder );

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
                this->odeSolverPostObservers( hcur, scResult.acceptStep, scResult.changeStepSize, stepTruncated, errorNorm, izfTrunc, transitionType, this );

                // Reduce step size, if step is accepted, a switch has occurred, and m_h_after_switch is nonzero and less than new step size.
                if( stepTruncated   &&   m_h_after_switch > 0   &&   m_h > m_h_after_switch )
                    m_h = m_h_after_switch;

                this->tstat4.add( timer.Lap() );

                // Stop the loop if the step is accepted
                if( scResult.acceptStep )
                    break;
                }
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result = OdeSolver<VD>::parameters();
            result["disable_error_check"] = m_disable_error_check;
            result["force_error_calc"] = m_force_error_calc;
            result["h_after_switch"] = m_h_after_switch;
            result["mode"] = m_mode == HighMode? "high": "low";
            return result;
            }

        void setParameters( const OptionalParameters::Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            OptionalParameters::maybeLoadParameter( parameters, "disable_error_check", m_disable_error_check );
            OptionalParameters::maybeLoadParameter( parameters, "force_error_calc", m_force_error_calc );
            OptionalParameters::maybeLoadParameter( parameters, "h_after_switch", m_h_after_switch );
            OptionalParameters::maybeSetParameter( parameters, "mode", [this](std::string v) {
                if( v == "high" )
                    m_mode = HighMode;
                else if( v == "low" )
                    m_mode = LowMode;
                else
                    throw cxx::exception( std::string("Invalid mode: '") + v + "'" );
                } );
            }

    private:
        typedef ExplicitRKHelper<VD> RKHelper;
        typedef typename RKHelper::Coefficients Coefficients;
        typedef typename RKHelper::Output Output;

        const Coefficients m_coefficients;
        const unsigned int m_highOrder;
        const unsigned int m_lowOrder;

        bool m_disable_error_check;
        bool m_force_error_calc;
        real_type m_h_after_switch;
        Mode m_mode;

        real_type m_initialTime;        // Initial time
        V m_initialState;               // Initial state

        real_type m_h;                  // Current step size

        Output m_output;

        void initStep()
            {
            if( !m_output.empty() )
                return;

            auto rhs = this->odeRhs();
            auto n = rhs->varCount();

            // Should already have an initial state
            ASSERT( m_initialState.size() == n );

            // Allocate memory
            auto stages = m_coefficients.c.size() + 1;
            m_output = Output( stages, n, 2 );

            m_h = this->initialStepSize();
            }

        void cleanupStep()
            {
            m_output = Output();
            if( auto ec = this->eventController() )
                ec->reset();
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_EXPLICIT_EMBEDDED_RK_H_

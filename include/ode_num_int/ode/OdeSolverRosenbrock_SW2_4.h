// OdeSolverRosenbrock_SW2_4.h

#ifndef _ODESOLVER_ODESOLVERROSENBROCK_SW2_4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERROSENBROCK_SW2_4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolverRosenbrock_W_base.h"
#include "./interfaces/OdeSolverErrorNormCalculator.h"
#include "./interfaces/OdeSolverStepSizeController.h"
#include "./interfaces/OdeSolverEventController.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverRosenbrock_SW2_4 :
    public OdeSolverRosenbrock_W_base<VD>,
    public OdeSolverErrorNormCalculator<VD>,
    public OdeSolverStepSizeController<VD>,
    public OdeSolverEventController<VD>,
    public FactoryMixin< OdeSolverRosenbrock_SW2_4<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverRosenbrock_SW2_4() :
            OdeSolverErrorNormCalculator<VD>( *this, "inf" ),
            OdeSolverStepSizeController<VD>( *this, "simple" ),
            OdeSolverEventController<VD>( *this, "linear" ),
            m_d( 1 ),
            m_disable_error_check( false ),
            m_initialTime( 0 ),
            m_h( 0 )
            {}

        void setInitialStepSize( real_type initialStepSize )
            {
            OdeSolver<VD>::setInitialStepSize( initialStepSize );
            m_h = initialStepSize;
            }

        int order() const {
            return 2;
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

            if( soft )
                // Compute ODE RHS at the beginning of the step
                this->odeRhs()->rhs( m_initialOdeRhs, m_initialTime, m_initialState );
            else
                cleanupStep();
            }

        void doStep()
            {
            bool needInitStep = true;

            while( true ) {
                this->odeSolverPreObservers( m_h, this );

                OpaqueTickCounter timer;    // deBUG

                timer.Start();
                if( needInitStep ) {
                    needInitStep = false;
                    initStep();
                    }

                auto rhs = this->odeRhs();

                this->tstat0.add( timer.Lap() );
                rhs->beforeStep( m_initialTime, m_initialState );
                rhs->beforeStep2( m_initialOdeRhs );

                auto ec = this->eventController();
                if( ec )
                    ec->atStepStart( m_initialState, m_initialTime );

                // k1
                this->copy( m_k1, m_initialOdeRhs );
                linSolve( m_k1 );

                this->tstat1.add( timer.Lap() );

                // k2
                this->copy( m_buf, m_initialState );
                m_buf += m_k1.scaled( m_h*2/3 );
                rhs->rhs( m_k2, m_initialTime + m_h*2/3, m_buf );
                this->mult( m_buf, m_k1 );
                m_k2 += m_buf.scaled( -4./3*m_h*m_d );
                linSolve( m_k2 );

                this->tstat2.add( timer.Lap() );

                // Next state
                this->copy( m_nextState, m_initialState );
                m_nextState += m_k1.scaled( 0.25*m_h );
                m_nextState += m_k2.scaled( 0.75*m_h );

                this->tstat3.add( timer.Lap() );

                // k3
                rhs->rhs( m_nextOdeRhs, m_initialTime + m_h, m_nextState );
                real_type errorNorm = 0;
                if( !m_disable_error_check ) {
                    this->copy( m_k3, m_nextOdeRhs );
                    linSolve( m_k3 );

                    this->tstat4.add( timer.Lap() );

                    // k4
                    this->copy( m_buf, m_nextState );
                    m_buf += m_k3.scaled( m_h*2/3 );
                    rhs->rhs( m_k4, m_initialTime + m_h*5/3, m_buf );
                    this->copy( m_buf2, m_k1.scaled(2./3) );
                    m_buf2 += m_k2.scaled( 6 );
                    this->mult( m_buf, m_buf2 );
                    m_k4 += m_buf.scaled( m_h*m_d );
                    linSolve( m_k4 );

                    this->tstat5.add( timer.Lap() );

                    // Error estimate
                    this->copy( m_buf, m_k3 );
                    m_buf -= m_k2;
                    m_buf *= 5;
                    m_buf += m_k1;
                    m_buf -= m_k4;
                    m_buf *= 0.125*m_h;
                    errorNorm = this->errorNormCalculator()->errorNorm( m_buf );
                }

                real_type nextTime = m_initialTime + m_h;
                auto hcur = m_h;
                auto scResult = m_disable_error_check ?
                            typename OdeStepSizeController<VD>::Result( m_h, true, false ) :
                            this->odeStepSizeController()->controlStepSize( m_h, errorNorm, 2 );

                this->tstat6.add( timer.Lap() );

                bool stepTruncated = false;
                unsigned int izfTrunc = ~0u;
                int transitionType = 0;
                if( scResult.acceptStep ) {
                    if( ec ) {
                        auto interpolate = linearInterpolator( m_initialState, m_nextState );
                        stepTruncated = ec->atStepEnd( m_initialState, m_nextState, nextTime, interpolate, &izfTrunc, &transitionType );
                        }
                    m_initialTime = nextTime;
                    m_initialState.swap( m_nextState );
                    if( stepTruncated ) {
                        this->odeRhs()->rhs( m_initialOdeRhs, m_initialTime, m_initialState );
                        // buildJacobian();
                        }
                    else
                        m_initialOdeRhs.swap( m_nextOdeRhs );
                    }
                if( scResult.changeStepSize ) {
                    m_h = scResult.newStepSize;
                    if( m_h <= this->stepSizeMinThreshold() )
                        throw cxx::exception( "Step size has decreased to the minimum or below" );
                    }

                this->odeSolverPostObservers( hcur, scResult.acceptStep, scResult.changeStepSize, stepTruncated, errorNorm, izfTrunc, transitionType, this );

                if( scResult.acceptStep )
                    break;

                this->tstat7.add( timer.Lap() );
                }
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolver<VD>::parameters();
            result["d"] = m_d;
            result["disable_error_check"] = m_disable_error_check;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            OptionalParameters::maybeLoadParameter( parameters, "d", m_d );
            OptionalParameters::maybeLoadParameter( parameters, "disable_error_check", m_disable_error_check );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolver<VD>::helpOnParameters();
            result["d"] = "Implicitness parameter: W = I - d*A, A is the approximate Jacobian";
            result["disable_error_check"] = "Disable error check and always use the initial step size";
            return result;
            }

    private:
        real_type m_d;                  // W-method implicitness parameter
        bool m_disable_error_check;

        real_type m_initialTime;        // Initial time
        V m_initialState;               // Initial state

        real_type m_h;                  // Current step size
        V m_initialOdeRhs;              // ODE RHS corresp. to the initial state
        V m_nextOdeRhs;                 // ODE RHS corresp. to the state at the end of the step
        V m_k1, m_k2, m_k3, m_k4;
        V m_nextState;
        V m_buf;
        V m_buf2;

        void initStep()
            {
            bool hard = m_k1.empty();

            if( hard ) {
                auto rhs = this->odeRhs();
                auto n = rhs->varCount();

                // Should already have an initial state
                ASSERT( m_initialState.size() == n );

                // Allocate memory
                m_initialOdeRhs.resize( n );
                m_nextOdeRhs.resize( n );
                m_k1.resize( n );
                m_k2.resize( n );
                m_k3.resize( n );
                m_k4.resize( n );
                m_nextState.resize( n );
                m_buf.resize( n );
                m_buf2.resize( n );

                // Compute ODE RHS at the beginning of the step
                this->odeRhs()->rhs( m_initialOdeRhs, m_initialTime, m_initialState );
                }

            OdeSolverRosenbrock_W_base<VD>::initStep(
                hard, m_initialTime, m_initialState, m_initialOdeRhs, m_h * m_d );
            }

        void cleanupStep() {
            m_k1.resize( 0 );
            }

        void linSolve( V& x ) {
            OdeSolverRosenbrock_W_base<VD>::linSolve( x, m_h * m_d );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESOLVERROSENBROCK_SW2_4_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

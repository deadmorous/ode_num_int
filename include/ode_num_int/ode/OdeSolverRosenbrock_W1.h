// OdeSolverRosenbrock_W1.h

#ifndef _ODESOLVER_ODESOLVERROSENBROCK_W1_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERROSENBROCK_W1_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolverRosenbrock_W_base.h"
#include "./interfaces/OdeSolverEventController.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverRosenbrock_W1 :
    public OdeSolverRosenbrock_W_base<VD>,
    public OdeSolverEventController<VD>,
    public FactoryMixin< OdeSolverRosenbrock_W1<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverRosenbrock_W1() :
            OdeSolverEventController<VD>( *this, "linear" ),
            m_d( 1 ),
            m_initialTime( 0 ),
            m_h( 0 )
            {}

        void setInitialStepSize( real_type initialStepSize )
            {
            OdeSolver<VD>::setInitialStepSize( initialStepSize );
            m_h = initialStepSize;
            }

        int order() const {
            return 1;
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
                cleanupStep();
            }

        void doStep()
            {
            this->odeSolverPreObservers( m_h, this );

            OpaqueTickCounter timer;    // deBUG

            timer.Start();
            initStep();

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

            // Next state
            this->copy( m_nextState, m_initialState );
            m_nextState += m_k1.scaled( m_h );

            this->tstat1.add( timer.Lap() );

            real_type nextTime = m_initialTime + m_h;
            bool stepTruncated = false;
            unsigned int izfTrunc = ~0u;
            int transitionType = 0;
            if( ec ) {
                auto interpolate = linearInterpolator( m_initialState, m_nextState );
                stepTruncated = ec->atStepEnd( m_initialState, m_nextState, nextTime, interpolate, &izfTrunc, &transitionType );
                }
            m_initialTime = nextTime;
            m_initialState.swap( m_nextState );

            this->odeSolverPostObservers( m_h, true /*acceptStep*/, false /*changeStepSize*/, stepTruncated, 0 /*errorNorm*/, izfTrunc, transitionType, this );

            this->tstat2.add( timer.Lap() );
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolver<VD>::parameters();
            result["d"] = m_d;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            OptionalParameters::maybeLoadParameter( parameters, "d", m_d );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolver<VD>::helpOnParameters();
            result["d"] = "Implicitness parameter: W = I - d*A, A is the approximate Jacobian";
            return result;
            }

    private:
        real_type m_d;                  // W-method implicitness parameter

        real_type m_initialTime;        // Initial time
        V m_initialState;               // Initial state

        real_type m_h;                  // Current step size
        V m_initialOdeRhs;              // ODE RHS corresp. to the initial state
        V m_k1;
        V m_nextState;

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
                m_k1.resize( n );
                m_nextState.resize( n );
                }

            // Compute ODE RHS at the beginning of the step
            this->odeRhs()->rhs( m_initialOdeRhs, m_initialTime, m_initialState );

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

#endif // _ODESOLVER_ODESOLVERROSENBROCK_W1_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

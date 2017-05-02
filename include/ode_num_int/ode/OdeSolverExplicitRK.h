// OdeSolverExplicitRK.h

#ifndef _ODE_ODESOLVEREXPLICITRK_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODESOLVEREXPLICITRK_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeSolver.h"
#include "./ExplicitRKHelper.h"
#include "./interfaces/OdeSolverEventController.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverExplicitRK :
    public OdeSolver<VD>,
    public OdeSolverEventController<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverExplicitRK(
                const typename ExplicitRKHelper<VD>::Coefficients& coefficients,
                unsigned int order ) :
            OdeSolverEventController<VD>( *this, "linear" ),
            m_coefficients( coefficients ),
            m_order( order ),
            m_initialTime( 0 )
            {}

        int order() const {
            return m_order;
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
            V& nextState = m_output.x2[0];

            // Compute step size, if error check is enabled
            real_type nextTime = m_initialTime + m_h;

            // Process events
            bool stepTruncated = false;
            unsigned int izfTrunc = ~0u;
            int transitionType = 0;
            if( ec ) {
                auto interpolate = linearInterpolator( m_initialState, nextState );
                stepTruncated = ec->atStepEnd( m_initialState, nextState, nextTime, interpolate, &izfTrunc, &transitionType );
                }
            m_initialTime = nextTime;
            m_initialState.swap( nextState );

            this->tstat2.add( timer.Lap() );

            // Invoke observers
            this->odeSolverPostObservers(
                OdeSolverPostObserverArg<VD>()
                    .setStepSize( m_h )
                    .setIzfTrunc( izfTrunc )
                    .setTransitionType( transitionType )
                    .setSolver( this ) );

            this->tstat4.add( timer.Lap() );
            }

    private:
        typedef ExplicitRKHelper<VD> RKHelper;
        typedef typename RKHelper::Coefficients Coefficients;
        typedef typename RKHelper::Output Output;

        const Coefficients m_coefficients;
        const unsigned int m_order;

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
            m_output = Output( stages, n, 1 );

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

#endif // _ODE_ODESOLVEREXPLICITRK_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// OdeSolverGragg.h

#ifndef _ODE_ODESOLVERGRAGG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODESOLVERGRAGG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeSolver.h"
#include "./interfaces/OdeSolverEventController.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverGragg :
    public OdeSolver<VD>,
    public OdeSolverEventController<VD>,
    public FactoryMixin< OdeSolverGragg<VD>, OdeSolver<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverGragg() :
            OdeSolverEventController<VD>( *this, "linear" ),
            m_smooth( false ),
            m_initialTime( 0 )
            {
            this->onOdeRhsChanged( [this]() { m_stepsRunning = false; } );
            }

        int order() const {
            return 2;
            }

        real_type initialTime() const {
            return m_initialTime;
            }

        V initialState() const
            {
            if( m_smooth && m_stepsRunning ) {
                V result = m_initialU;
                result *= 0.5;
                result += const_cast<V&>(m_initialV).scaled( 0.5 );
                return result;
                }
            else
                return m_initialU;
            }

        void setInitialStepSize( real_type initialStepSize )
            {
            OdeSolver<VD>::setInitialStepSize( initialStepSize );
            m_stepsRunning = false;
            }

        void setInitialState( real_type initialTime, const V& initialState, bool /*soft*/ = false )
            {
            m_initialTime = initialTime;
            m_initialU = initialState;
            m_stepsRunning = false;
            }

        void doStep()
            {
            auto h = this->initialStepSize();
            this->odeSolverPreObservers( h, this );
            OpaqueTickCounter timer;    // deBUG
            timer.Start();

            {
                auto nv = m_initialU.size();
                m_fbuf.resize( nv );
                m_nextU.resize( nv );
            }

            auto rhs = this->odeRhs();
            auto h2 = h / 2;
            rhs->beforeStep( m_initialTime, m_initialU );
            auto ec = this->eventController();
            if( ec )
                ec->atStepStart( m_initialU, m_initialTime );

            this->tstat0.add( timer.Lap() );

            if( !m_stepsRunning ) {
                m_initialV = m_initialU;
                rhs->rhs( m_fbuf, m_initialTime, m_initialU );
                }
            m_initialV += m_fbuf.scaled( h2 );
            rhs->rhs( m_fbuf, m_initialTime + h2, m_initialV );
            m_nextU = m_initialU;
            m_nextU += m_fbuf.scaled( h );
            rhs->rhs( m_fbuf, m_initialTime + h, m_nextU );
            m_initialV += m_fbuf.scaled( h2 );

            this->tstat1.add( timer.Lap() );

            // Process events
            bool stepTruncated = false;
            unsigned int izfTrunc = ~0u;
            int transitionType = 0;
            real_type nextTime = m_initialTime + h;
            if( ec ) {
                auto interpolate = linearInterpolator( m_initialU, m_nextU );
                stepTruncated = ec->atStepEnd( m_initialU, m_nextU, nextTime, interpolate, &izfTrunc, &transitionType );
                }
            m_initialTime = nextTime;
            m_initialU.swap( m_nextU );
            m_stepsRunning = !stepTruncated;

            this->tstat2.add( timer.Lap() );

            // Invoke observers
            this->odeSolverPostObservers(
                OdeSolverPostObserverArg<VD>()
                    .setStepSize( h )
                    .setStepTruncated( stepTruncated )
                    .setIzfTrunc( izfTrunc )
                    .setTransitionType( transitionType )
                    .setSolver( this ) );

            this->tstat3.add( timer.Lap() );
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolver<VD>::parameters();
            result["smooth"] = m_smooth;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolver<VD>::setParameters( parameters );
            OptionalParameters::maybeLoadParameter( parameters, "smooth", m_smooth );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolver<VD>::helpOnParameters();
            result["smooth"] = "Return smoothened current state";
            return result;
            }

    private:
        bool m_smooth;

        real_type m_initialTime;
        V m_initialU;
        V m_initialV;
        V m_fbuf;
        V m_nextU;
        cxx::bool0 m_stepsRunning;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_ODESOLVERGRAGG_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

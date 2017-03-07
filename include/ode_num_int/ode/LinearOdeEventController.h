// LinearOdeEventController.h

#ifndef _ODESOLVER_LINEARODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_LINEARODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeEventController.h"

namespace ctm {
namespace math {

template< class VD >
class LinearOdeEventController :
    public OdeEventController<VD>,
    public FactoryMixin< LinearOdeEventController<VD>, OdeEventController<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef std::function<void(V&, real_type)> Interpolator;

        LinearOdeEventController() :
            m_clean( true ),
            m_haveZf( false ),
            m_t1()
            {}

        void reset() {
            m_clean = true;
            }

        void atStepStart( const V& x1, real_type t1 )
            {
            m_t1 = t1;
            if( init() ) {
                if( m_haveZf ) {
                    // Compute event indicators at step start
                    auto rhs = this->odeRhs();
                    rhs->zeroFunctions( m_zf1, t1, x1 );
                    }
                }
            }

        bool atStepEnd(
                const V& x1, V& x2, real_type& t2, Interpolator /*interpolate*/,
                unsigned int *izfTrunc = nullptr, int *transitionType = nullptr )
            {
            ASSERT( !m_clean );
            auto rhs = this->odeRhs();
            if( !m_haveZf )
                return rhs->truncateStep( t2, x2 );

            // Compute event indicators at step end
            rhs->zeroFunctions( m_zf2, t2, x2 );

            // Find indices of zero functions that changed their sign
            auto nz = m_zf1.size();
            unsigned int nx = 0;
            for( decltype(nz) i=0; i<nz; ++i )
                if( xz( i ) )
                    m_zfi[nx++] = i;
            m_zfi[nx] = ~0u;

            if( nx == 0 ) {
                // No event indicators changed sign
                m_zf1.swap( m_zf2 );
                return false;
                }

            // Approximate the time of switching by using linear interpolation
            real_type tmin = 1;
            unsigned int itmin = ~0u;
            for( unsigned int ix=0; ix<nx; ++ix ) {
                auto i = m_zfi[ix];
                auto zf1 = m_zf1[i];
                auto zf2 = m_zf2[i];
                real_type t = zf1 / ( zf1 - zf2 );
                if( tmin > t ) {
                    tmin = t;
                    itmin = i;
                    }
                }
            ASSERT( itmin != ~0u );

            // Linearly interpolate state vector
            real_type h = t2 - m_t1;
            t2 = m_t1 + h*tmin;
            x2 *= tmin;
            x2 += const_cast<V&>(x1).scaled( 1. - tmin );   // have to use const_cast because of lack of design

            // Switch state
            std::fill( m_transitions.begin(), m_transitions.end(), 0 );
            m_transitions[itmin] = m_state[itmin] = s( m_zf2[itmin] );
            rhs->switchPhaseState( m_transitions.data() );
            if( izfTrunc )
                *izfTrunc = itmin;
            if( transitionType )
                *transitionType = s( m_zf2[itmin] );

            // Compute m_zf1 using linear interpolation
            for( unsigned int i=0; i<nz; ++i ) {
                auto& zf = m_zf1[i];
                zf = zf*( 1. - tmin ) + m_zf2[i]*tmin;
                }

            return true;
            }

    private:
        bool m_clean;
        bool m_haveZf;                  // True when there are more than zero zero functions
        V m_zf1;                        // Zero functions at the beginning of the step
        V m_zf2;                        // Zero functions at the end of the step
        V m_zfbuf;                      // Zero functions somewhere in the middle
        std::vector<unsigned int> m_zfi;// Indices of zero function that changed their sign, terminated with ~0u
        std::vector<int> m_transitions;
        std::vector<int> m_state;
        real_type m_t1;

        bool init()
            {
            if( !m_clean )
                return false;
            m_clean = false;
            auto rhs = this->odeRhs();
            auto nz = rhs->zeroFuncCount();
            m_haveZf = nz > 0;
            m_zf1.resize( nz );
            m_zf2.resize( nz );
            m_zfbuf.resize( nz );
            m_zfi.resize( nz + 1 );
            m_transitions.resize( nz );
            m_state.resize( nz );
            std::fill( m_state.begin(), m_state.end(), 0 );
            return true;
            }

        static int s( real_type x ) {
            return x < 0? -1: x > 0? 1: 0;
            }

        bool xz( unsigned int i ) const
            {
            auto x1 = m_zf1[i];
            auto x2 = m_zf2[i];
            auto st = m_state[i];
            if( x2 > 0 )
                return st == 1 ?   false :   x1 < 0;
            else
                return st == -1 ?   false :   x1 > 0;
            }

    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_LINEARODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

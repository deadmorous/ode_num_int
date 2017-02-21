// ode_explicit_rk_helper.h

#ifndef _ODE_EXPLICIT_RK_HELPER_H_
#define _ODE_EXPLICIT_RK_HELPER_H_

#include "ode_time_stepper.h"

namespace ctm {
namespace math {

template< class VD >
class ExplicitRKHelper
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef std::vector<V> VV;

        struct Coefficients
            {
                std::vector< real_type > a;
                std::vector< real_type > b;
                std::vector< real_type > c;
                int last_k_arg_as_x2_index;
                Coefficients() {}
                Coefficients(
                        const std::vector< real_type >& a,
                        const std::vector< real_type >& b,
                        const std::vector< real_type >& c,
                        int last_k_arg_as_x2_index ) :
                    a( a ),
                    b( b ),
                    c( c ),
                    last_k_arg_as_x2_index( last_k_arg_as_x2_index )
                    {}
            };

        struct Output
            {
                VV k;
                VV x2;
                V buf;
                Output() :
                    x2( 0 ),
                    buf( 0 )
                    {}

                Output( unsigned int stages,
                        unsigned int varCount,
                        unsigned int x2Count ) :
                    k( stages, V(varCount) ),
                    x2( x2Count, V(varCount) ),
                    buf( varCount )
                    {}
                bool empty() const {
                    return k.empty();
                    }
            };

        static void compute( Output& o, const Coefficients& c, const OdeRhs<VD> *rhs, const V& x1, real_type t1, real_type h )
            {
            auto stages = o.k.size();
            auto x2Count = o.x2.size();
            ASSERT( c.c.size() + 1 == stages );
            ASSERT( c.b.size() == stages * x2Count );
            ASSERT( c.a.size() == stages * (stages-1) / 2 );
            rhs->rhs( o.k[0], t1, x1 );
            rhs->beforeStep2( o.k[0] );
            auto a = c.a.data();
            const real_type zero = static_cast<real_type>( 0 );
            for( decltype(stages) stage=1; stage<stages; ++stage ) {
                copy( o.buf, x1 );
                for( decltype(stages) i=0; i<stage; ++i, ++a )
                    if( *a != zero )
                        o.buf += o.k[i].scaled( h * *a );
                rhs->rhs( o.k[stage], t1 + h * c.c[stage-1], o.buf );
                }

            auto b = c.b.data();
            for( decltype(x2Count) ix=0; ix<x2Count; ++ix ) {
                auto& x2 = o.x2[ix];
                if( ix == c.last_k_arg_as_x2_index ) {
                    o.buf.swap( x2 );
                    b += stages;
                    }
                else {
                    copy( x2, x1 );
                    for( decltype(stages) stage=0; stage<stages; ++stage, ++b ) {
                        if( *b != zero )
                            x2 += o.k[stage].scaled( h * *b );
                        }
                    }
                }
            }

        template< class VDdst, class VDsrc >
        static void copy( VectorTemplate<VDdst>& dst, const VectorTemplate<VDsrc>& src )
            {
            ASSERT( dst.size() == src.size() );
            std::copy( src.begin(), src.end(), dst.begin() );
            }

        template< class VDdst, class VDsrc >
        static void copy( VectorTemplate<VDdst>&& dst, const VectorTemplate<VDsrc>& src )
            {
            ASSERT( dst.size() == src.size() );
            std::copy( src.begin(), src.end(), dst.begin() );
            }

    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_EXPLICIT_RK_HELPER_H_

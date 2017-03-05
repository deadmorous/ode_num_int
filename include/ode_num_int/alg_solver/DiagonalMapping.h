// DiagonalMapping.h

#ifndef _ALGSOLVER_DIAGONALMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_DIAGONALMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../VectorBijectiveMapping.h"

namespace ctm {
namespace math {

template< class VD >
class DiagonalMapping : public VectorBijectiveMapping< VD >
    {
    public:
        typedef VectorTemplate< VD > V;

        explicit DiagonalMapping( const V& d ) : m_d(d) {}

        unsigned int inputSize() const {
            return m_d.size();
            }

        unsigned int outputSize() const {
            return m_d.size();
            }

        void map( V& dst, const V& x ) const
            {
            ASSERT( x.size() == m_d.size() );
            auto n = x.size();
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[i] * m_d[i];
            }

        void unmap( V& dst, const V& x ) const
            {
            ASSERT( x.size() == m_d.size() );
            auto n = x.size();
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[i] / m_d[i];
            }

    private:
        V m_d;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        DiagonalMappingHolder, DiagonalMapping,
        diagonalMapping, setDiagonalMapping,
        onDiagonalMappingChanged, offDiagonalMappingChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_DIAGONALMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// VectorBijectiveMapping.h

#ifndef _ALG_INTERFACES_VECTORBIJECTIVEMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_VECTORBIJECTIVEMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./VectorMapping.h"

namespace ctm {
namespace math {

template< class VD > class VectorInverseMapping;

template< class VD >
class VectorBijectiveMapping : public VectorMapping<VD>
    {
    public:
        typedef VectorTemplate<VD> V;

        virtual void unmap( V& dst, const V& x ) const = 0;

        V unmap( const V& x ) const {
            V result( this->inputSize() );
            unmap( result, x );
            return result;
            }

        inline VectorInverseMapping<VD> inv() const;
    };

template< class VD >
class VectorInverseMapping : public VectorBijectiveMapping<VD>
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef std::shared_ptr< VectorInverseMapping<VD> > Ptr;

        explicit VectorInverseMapping( const VectorBijectiveMapping<VD>& bijection ) :
            m_bijection( bijection )
            {}

        unsigned int inputSize() const {
            return m_bijection.outputSize();
            }

        unsigned int outputSize() const {
            return m_bijection.inputSize();
            }

        void map( V& dst, const V& x ) const {
            m_bijection.unmap( dst, x );
            }

        virtual void unmap( V& dst, const V& x ) const {
            m_bijection.map( dst, x );
            }

    private:
        const VectorBijectiveMapping<VD>& m_bijection;
    };

template< class VD >
inline VectorInverseMapping<VD> VectorBijectiveMapping<VD>::inv() const {
    return VectorInverseMapping<VD>( *this );
    }

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_VECTORBIJECTIVEMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

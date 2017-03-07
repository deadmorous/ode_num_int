// VectorNarrowingMapping.h

#ifndef _ALG_VECTORNARROWINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_VECTORNARROWINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/VectorMapping.h"
#include "../infra/cxx_exception.h"

namespace ctm {
namespace math {

template< class VD >
class VectorNarrowingMapping :
    public VectorMapping<VD>,
    public MappingHolder<VD>
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;

        std::vector< unsigned int > disabledInputIndices() const {
            return m_disabledInputIndices;
            }

        V stateForInputNarrowing() const {
            return m_state;
            }

        void setInputNarrowing( const std::vector< unsigned int >& disabledIndices, const V& state )
            {
            checkIndices( disabledIndices, this->mapping()->inputSize(), "setInputNarrowing" );
            m_disabledInputIndices = normalizedIndices( disabledIndices );
            m_state = state;
            }

        void disabledOutputIndices() const {
            return m_disabledOutputIndices;
            }

        void setOutputNarrowing( const std::vector< unsigned int >& disabledIndices )
            {
            checkIndices( disabledIndices, this->mapping()->outputSize(), "setOutputNarrowing" );
            m_disabledOutputIndices = normalizedIndices( disabledIndices );
            }

        void narrowInput( V& dst, const V& src ) const {
            reduce( dst, src, m_disabledInputIndices );
            }

        V narrowInput( const V& src ) const {
            return reduce( src, m_disabledInputIndices );
            }

        void widenInput( V& dst, const V& src ) const
            {
            if( m_disabledInputIndices.empty() )
                dst = src;
            else {
                auto ns = this->mapping()->inputSize();
                dst.resize( ns );
                unsigned int is = 0,   in = 0,   id = 0;
                for( ; is<ns; ++is ) {
                    for( ; id<m_disabledInputIndices.size() && m_disabledInputIndices[id]<is; ++id ) {}
                    if( id<m_disabledInputIndices.size() && m_disabledInputIndices[id]==is )
                        dst[is] = m_state[is];
                    else
                        dst[is] = src[in++];
                    }
                ASSERT( id == m_disabledInputIndices.size() || id+1 == m_disabledInputIndices.size() );
                ASSERT( in == src.size() );
                }
            }

        V widenInput( const V& src ) const {
            V result;
            widenInput( result, src );
            return result;
            }

        void narrowOutput( V& dst, const V& src ) const {
            reduce( dst, src, m_disabledOutputIndices );
            }

        V narrowOutput( const V& src ) const {
            return reduce( src, m_disabledOutputIndices );
            }

        unsigned int inputSize() const {
            return this->mapping()->inputSize() - m_disabledInputIndices.size();
            }

        unsigned int outputSize() const {
            return this->mapping()->outputSize() - m_disabledOutputIndices.size();
            }

        void map( V& dst, const V& x ) const
            {
            V xs, dsts;
            const V *px;
            V *pdst;
            if( m_disabledInputIndices.empty() )
                px = &x;
            else {
                widenInput( xs, x );
                px = &xs;
                }
            if( m_disabledOutputIndices.empty() )
                pdst = &dst;
            else {
                auto ns = this->mapping()->outputSize();
                dsts.resize( ns );
                pdst = &dsts;
                }
            this->mapping()->map( *pdst, *px );

            if( !m_disabledOutputIndices.empty() )
                reduce( dst, dsts, m_disabledInputIndices );
            }

        std::vector<unsigned int> inputIds() const {
            return reduce( this->mapping()->inputIds(), m_disabledInputIndices );
            }
        std::vector<unsigned int> outputIds() const {
            return reduce( this->mapping()->outputIds(), m_disabledOutputIndices );
            }

    private:
        std::vector< unsigned int > m_disabledInputIndices;
        std::vector< unsigned int > m_disabledOutputIndices;
        V m_state;

        static std::vector<unsigned int> normalizedIndices( const std::vector<unsigned int>& indices )
            {
            std::vector<unsigned int> result = indices;
            std::sort( result.begin(), result.end() );
            result.resize(
                        std::unique( result.begin(), result.end() ) -
                        result.begin() );
            return result;
            }

        static void checkIndices( const std::vector<unsigned int>& indices, unsigned int size, const char *methodName )
            {
            for( auto index : indices ) {
                if( index >= size )
                    throw cxx::exception( std::string("VectorNarrowingMapping::") + methodName + "() failed: invalid index" );
                }
            }

        template< class T >
        static void reduce( T& result, const T& x, const std::vector< unsigned int >& disabledIndices )
            {
            if( disabledIndices.empty() )
                result = x;
            else {
                auto nn = x.size() - disabledIndices.size();
                result.resize( nn );
                unsigned int is = 0,   in = 0,   id = 0;
                for( ; in<nn; ++in ) {
                    while( true ) {
                        for( ; id<disabledIndices.size() && disabledIndices[id]<is; ++id ) {}
                        if( id<disabledIndices.size() && disabledIndices[id]==is )
                            ++is;
                        else {
                            result[in] = x[is++];
                            break;
                            }
                        }
                    }
                ASSERT( id < disabledIndices.size()   ||   is == x.size() );
                }
            }

        template< class T >
        static T reduce( const T& x, const std::vector< unsigned int >& disabledIndices )
            {
            if( disabledIndices.empty() )
                return x;
            T result;
            reduce( result, x, disabledIndices );
            return result;
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_VECTORNARROWINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

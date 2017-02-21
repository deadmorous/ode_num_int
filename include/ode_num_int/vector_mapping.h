// vector_mapping.h

#ifndef _VECTOR_MAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _VECTOR_MAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "vector_util.h"
#include "ctmstd/def_prop_class.h"
#include "ctmstd/cxx_exception.h"
#include "ctmstd/cxx_observers.h"
#include "factory.h"
#include "opt_param.h"

namespace ctm {
namespace math {

template< class VD > class VectorMapping;

template< class VD >
using VectorMappingPreObservers = cxx::Observers< const VectorTemplate< VD >& /*argument*/, const VectorMapping<VD>* /*mapping*/ >;

template< class VD >
using VectorMappingPostObservers = cxx::Observers< const VectorTemplate< VD >& /*argument*/, const VectorTemplate< VD >& /*value*/, const VectorMapping<VD>* /*mapping*/ >;

template< class VD >
class VectorMapping :
    public OptionalParameters
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;
        VectorMappingPreObservers<VD> vectorMappingPreObservers;
        VectorMappingPostObservers<VD> vectorMappingPostObservers;

        virtual ~VectorMapping() {}

        virtual unsigned int inputSize() const = 0;
        virtual unsigned int outputSize() const = 0;
        virtual void map( V& dst, const V& x ) const = 0;
        virtual std::vector<unsigned int> inputIds() const {
            return std::vector<unsigned int>( inputSize(), 0 );
            }
        virtual std::vector<unsigned int> outputIds() const {
            return std::vector<unsigned int>( outputSize(), 0 );
            }

        V map( const V& x ) const {
            V result( outputSize() );
            map( result, x );
            return result;
            }
        V operator()( const V& x ) const {
            return map( x );
            }
    };

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

#define CTM_DEF_PROP_VD_TEMPLATE_CLASS(ClassName, StoredClassName, getter, setter) \
    template< class VD > CTM_DEF_PROP_CLASS( \
        ClassName, \
        std::shared_ptr< StoredClassName<VD> >, std::shared_ptr< StoredClassName<VD> >, \
        getter, setter )

#define CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(ClassName, StoredClassName, getter, setter, addOnChangeObserver, removeOnChangeObserver) \
    template< class VD > CTM_DEF_NOTIFIED_PROP_CLASS( \
        ClassName, \
        std::shared_ptr< StoredClassName<VD> >, std::shared_ptr< StoredClassName<VD> >, \
        getter, setter, addOnChangeObserver, removeOnChangeObserver )

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(MappingHolder, VectorMapping, mapping, setMapping, onMappingChanged, offMappingChanged)



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

template< class VD >
class VectorReorderingMapping :
    public VectorMapping<VD>,
    public MappingHolder<VD>,
    public Factory< VectorReorderingMapping<VD> >
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;

        std::vector< unsigned int > inputOrdering() const
            {
            maybeInit();
            return m_inputOrdering;
            }

        void setInputOrdering( const std::vector< unsigned int >& inputOrdering ) {
            m_inputOrdering = inputOrdering;
            }

        std::vector< unsigned int > outputOrdering() const {
            maybeInit();
            return m_outputOrdering;
            }

        void setOutputOrdering( const std::vector< unsigned int >& outputOrdering ) {
            m_outputOrdering = outputOrdering;
            }

        unsigned int inputSize() const
            {
            maybeInit();
            return this->mapping()->inputSize();
            }

        unsigned int outputSize() const {
            maybeInit();
            return this->mapping()->outputSize();
            }

        template< class Container >
        void orderInput( Container& dst, const Container& x ) const
            {
            maybeInit();
            order( dst, x, m_inputOrdering );
            }

        template< class Container >
        Container orderInput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            order( dst, x, m_inputOrdering );
            return dst;
            }

        template< class Container >
        void unorderInput( Container& dst, const Container& x ) const
            {
            maybeInit();
            unorder( dst, x, m_inputOrdering );
            }

        template< class Container >
        Container unorderInput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            unorder( dst, x, m_inputOrdering );
            return dst;
            }

        template< class Container >
        void orderOutput( Container& dst, const Container& x ) const
            {
            maybeInit();
            order( dst, x, m_outputOrdering );
            }

        template< class Container >
        Container orderOutput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            order( dst, x, m_outputOrdering );
            return dst;
            }

        template< class Container >
        void unorderOutput( Container& dst, const Container& x ) const
            {
            maybeInit();
            unorder( dst, x, m_outputOrdering );
            }

        template< class Container >
        Container unorderOutput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            unorder( dst, x, m_outputOrdering );
            return dst;
            }

        void map( V& dst, const V& x ) const
            {
            V xu, dstu;
            unorderInput( xu, x );
            this->mapping()->map( dstu, xu );
            orderOutput( dst, dstu );
            }

        std::vector<unsigned int> inputIds() const
            {
            std::vector<unsigned int> io;
            orderInput( io, this->mapping()->inputIds() );
            return io;
            }

        std::vector<unsigned int> outputIds() const {
            std::vector<unsigned int> io;
            orderOutput( io, this->mapping()->outputIds() );
            return io;
            }

    protected:
        virtual void maybeInit() {}

        void maybeInit() const {
            const_cast< VectorReorderingMapping<VD>* >( this )->maybeInit();
            }

    private:
        std::vector< unsigned int > m_inputOrdering;
        std::vector< unsigned int > m_outputOrdering;

        template< class Container >
        void order( Container& dst, const Container& x, const std::vector<unsigned int> ordering ) const
            {
            auto n = ordering.size();
            ASSERT( x.size() == n );
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[ordering[i]];
            }

        template< class Container >
        void unorder( Container& dst, const Container& x, const std::vector<unsigned int> ordering ) const
            {
            auto n = ordering.size();
            ASSERT( x.size() == n );
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[ordering[i]] = x[i];
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _VECTOR_MAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

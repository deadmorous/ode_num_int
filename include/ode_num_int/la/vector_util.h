// vector_util.h

#ifndef _LA_VECTOR_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _LA_VECTOR_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

#include "../infra/cxx_assert.h"
#include "../infra/cxx_mock_ptr.h"
#include "../util/m_inline.h"

namespace ctm {
namespace math {

template< class ElementType >
class VectorData
    {
    public:
        typedef VectorData< ElementType > vector_data_type;
        typedef ElementType value_type;
        typedef std::vector< ElementType > Data;
        typedef typename Data::size_type size_type;
        typedef typename Data::iterator iterator;
        typedef typename Data::reference reference;
        typedef typename Data::const_reference const_reference;
        typedef typename Data::const_iterator const_iterator;

        VectorData() {}
        explicit VectorData( size_type size ) : m_data( size ) {}
        explicit VectorData( const Data& data ) : m_data( data ) {}

        void swap( vector_data_type& that ) {
            std::swap( m_data, that.m_data );
            }

        reference operator[]( unsigned int index ) {
            return m_data.at( index );
            }

        const_reference operator[]( unsigned int index ) const {
            return m_data.at( index );
            }

        reference at( unsigned int index ) {
            return m_data.at( index );
            }

        const_reference at( unsigned int index ) const {
            return m_data.at( index );
            }

        iterator begin() {
            return m_data.begin();
            }
        const_iterator cbegin() const {
            return m_data.begin();
            }
        const_iterator begin() const {
            return m_data.begin();
            }

        iterator end() {
            return m_data.end();
            }
        const_iterator cend() const {
            return m_data.end();
            }
        const_iterator end() const {
            return m_data.end();
            }

        void resize( unsigned int size ) {
            m_data.resize( size, value_type() );
            }

        void clear() {
            std::fill( m_data.begin(), m_data.end(), value_type() );
            }

        size_type size() const {
            return m_data.size();
            }

        Data& data() {
            return m_data;
            }

        template< class ThatD >
        void assign( const ThatD& that )
            {
            resize( that.size() );
            std::copy( that.begin(), that.end(), begin() );
            }

    private:
        Data m_data;
    };

namespace VectorProxy {

template< class D >
class Block
    {
    public:
        typedef typename D::vector_data_type vector_data_type;
        typedef typename D::value_type value_type;
        typedef typename D::size_type size_type;
        typedef typename D::iterator iterator;
        typedef typename D::reference reference;
        typedef typename D::const_reference const_reference;
        typedef typename D::const_iterator const_iterator;

        Block( D& source, size_type from, size_type size ) :
            m_source( source ), m_from( from ), m_size( size )
            {
            }

        reference operator[]( unsigned int index ) {
            ASSERT( index < m_size );
            return m_source.at( index + m_from );
            }

        const_reference operator[]( unsigned int index ) const {
            ASSERT( index < m_size );
            return m_source.at( index + m_from );
            }

        reference at( unsigned int index ) {
            ASSERT( index < m_size );
            return m_source.at( index + m_from );
            }

        const_reference at( unsigned int index ) const {
            ASSERT( index < m_size );
            return m_source.at( index + m_from );
            }

        iterator begin() {
            return m_source.begin() + m_from;
            }
        const_iterator cbegin() const {
            return m_source.begin() + m_from;
            }
        const_iterator begin() const {
            return m_source.begin() + m_from;
            }

        iterator end() {
            return m_source.begin() + ( m_from + m_size );
            }
        const_iterator cend() const {
            return m_source.begin() + ( m_from + m_size );
            }
        const_iterator end() const {
            return m_source.begin() + ( m_from + m_size );
            }

        void clear() {
            std::fill( begin(), end(), value_type() );
            }

        size_type size() const {
            return m_size;
            }

        template< class ThatD >
        void assign( const ThatD& that )
            {
            ASSERT( size() == that.size() );
            std::copy( that.begin(), that.end(), begin() );
            }

    private:
        D& m_source;
        size_type m_from;
        size_type m_size;
    };

template< class D >
class Scale
    {
    public:
        typedef typename D::vector_data_type vector_data_type;
        typedef typename D::value_type value_type;
        typedef typename D::size_type size_type;
        typedef typename D::value_type reference;
        typedef typename D::value_type const_reference;

        template< class EmbeddedIterator >
        class iterator_template : public std::iterator<
                std::forward_iterator_tag,
                value_type,
                std::ptrdiff_t,
                cxx::mock_ptr< value_type >,
                value_type >
            {
            friend class Scale<D>;
            public:
                typedef iterator_template< EmbeddedIterator > ThisClass;
                typedef cxx::mock_ptr< value_type > pointer;
                const EmbeddedIterator& unwrap() const {
                    return m_it;
                    }
                value_type operator*() const {
                    return value_type( *m_it * m_scaleFactor );
                    }

                pointer operator->() const {
                    return pointer( operator*() );
                    }

                template< class E2 >
                ThisClass& operator=( const iterator_template< E2 >& that ) {
                    m_it = that.unwrap();
                    return *this;
                    }

                template< class E2 >
                bool operator==( const iterator_template< E2 >& that ) const {
                    return m_it == that.unwrap();
                    }

                template< class E2 >
                bool operator!=( const iterator_template< E2 >& that ) const {
                    return m_it != that.unwrap();
                    }

                iterator_template& operator++()
                    {
                    ++m_it;
                    return *this;
                    }

                template< class E2 >
                iterator_template( const iterator_template< E2 >& that ) :
                    m_it( that.unwrap() ),
                    m_scaleFactor( that.m_scaleFactor )
                    {}
            private:
                private: EmbeddedIterator m_it;
                value_type m_scaleFactor;

                iterator_template( const EmbeddedIterator& it, value_type scaleFactor ) :
                    m_it( it ), m_scaleFactor( scaleFactor )
                    {
                    }
            };

        typedef iterator_template< typename D::const_iterator > const_iterator;
        typedef iterator_template< typename D::const_iterator > iterator;

        Scale( const D& source, value_type scaleFactor ) :
            m_source( source ), m_scaleFactor( scaleFactor )
            {}

        value_type operator[]( unsigned int index ) const {
            ASSERT( index < size() );
            return m_source.at( index ) * m_scaleFactor;
            }

        value_type at( unsigned int index ) const {
            ASSERT( index < size() );
            return m_source.at( index ) * m_scaleFactor;
            }

        const_iterator cbegin() const {
            return const_iterator( m_source.begin(), m_scaleFactor );
            }
        const_iterator begin() const {
            return const_iterator( m_source.begin(), m_scaleFactor );
            }

        const_iterator cend() const {
            return const_iterator( m_source.end(), m_scaleFactor );
            }
        const_iterator end() const {
            return const_iterator( m_source.end(), m_scaleFactor );
            }

        size_type size() const {
            return m_source.size();
            }

    private:
        const D& m_source;
        value_type m_scaleFactor;
    };

} // namespace VectorProxy


template< class D > class VectorTemplate;

template< class ElementType >
using Vector = VectorTemplate< VectorData< ElementType > >;

template< class D >
class VectorTemplate : public D
    {
    public:
        typedef VectorTemplate<D> ThisClass;
        typedef typename D::vector_data_type vector_data_type;
        typedef typename D::value_type value_type;
        typedef typename D::size_type size_type;

        VectorTemplate() {}
        VectorTemplate( size_type size ) : D( size ) {}
        explicit VectorTemplate( const D& data ) : D( data ) {}
        explicit VectorTemplate( D&& data ) : D( std::move(data) ) {}

        template< class ThatD >
        VectorTemplate( const VectorTemplate< ThatD >& that ) : D( that.size() ) {
            std::copy( that.begin(), that.end(), this->begin() );
            }

        bool empty() const {
            return this->size() == 0;
            }

        template< class ThatD >
        ThisClass& operator=( const VectorTemplate< ThatD >& that )
            {
            this->assign( that );
            return *this;
            }

        ThisClass& operator=( const ThisClass& that )
            {
            if( &that == this )
                return *this;
            else {
                this->assign( that );
                return *this;
                }
            }

        template< class ThatD >
        ThisClass& operator+=( const VectorTemplate< ThatD >& that )
            {
            ASSERT( this->size() == that.size() );
            for( size_type i=0, n=this->size(); i<n; ++i )
                this->at( i ) += that.at( i );
            return *this;
            }

        template< class ThatD >
        ThisClass& operator-=( const VectorTemplate< ThatD >& that )
            {
            ASSERT( this->size() == that.size() );
            for( size_type i=0, n=this->size(); i<n; ++i )
                this->at( i ) -= that.at( i );
            return *this;
            }

        ThisClass& operator*=( value_type that )
            {
            for( size_type i=0, n=this->size(); i<n; ++i )
                this->at( i ) *= that;
            return *this;
            }

        template< class ThatD >
        value_type dot( const VectorTemplate< ThatD >& that ) const
            {
            ASSERT( this->size() == that.size() );
            value_type result = value_type();
            for( size_type i=0, n=this->size(); i<n; ++i )
                result += this->at(i) * that.at(i);
            return result;
            }

        VectorTemplate< VectorProxy::Block< D > > block( size_type from, size_type size )
            {
            ASSERT( from + size <= this->size() );
            return VectorTemplate< VectorProxy::Block< D > >(
                        VectorProxy::Block< D >( *this, from, size ) );
            }

        VectorTemplate< VectorProxy::Block< const D > > block( size_type from, size_type size ) const
            {
            ASSERT( from + size <= this->size() );
            return VectorTemplate< VectorProxy::Block< const D > >(
                        VectorProxy::Block< const D >( *this, from, size ) );
            }

        VectorTemplate< VectorProxy::Scale< D > > scaled( value_type scaleFactor )
            {
            return VectorTemplate< VectorProxy::Scale< D > >(
                        VectorProxy::Scale< D >( *this, scaleFactor ) );
            }

        VectorTemplate<vector_data_type> clone() const {
            return VectorTemplate<vector_data_type>( *this );
            }

        value_type infNorm() const
            {
            value_type result = value_type ();
            for( const auto& v : *this )
                result = std::max( result, std::fabs( v ) );
            return result;
            }

        value_type oneNorm() const
            {
            value_type result = value_type ();
            for( const auto& v : *this )
                result += std::fabs( v );
            return result;
            }

        value_type euclideanNormSquare() const
            {
            value_type result = value_type ();
            for( const auto& v : *this )
                result += v*v;
            return result;
            }

        value_type euclideanNorm() const {
            return std::sqrt( euclideanNormSquare() );
            }

        template< class ThatD >
        VectorTemplate<vector_data_type> operator+( const VectorTemplate< ThatD >& that ) const
            {
            auto result = clone();
            result += that;
            return result;
            }

        template< class ThatD >
        VectorTemplate<vector_data_type> operator-( const VectorTemplate< ThatD >& that ) const
            {
            auto result = clone();
            result -= that;
            return result;
            }

        std::string toString( unsigned int sizeHalfLimit = ~0u ) const
            {
            std::ostringstream s;
            s << '[';
            for( unsigned int i=0, n=this->size(); i<n; ) {
                if( i > 0 )
                    s << ", ";
                if( i == sizeHalfLimit && n > 2*sizeHalfLimit ) {
                    s << "...";
                    i = n - sizeHalfLimit;
                    continue;
                    }
                s << (*this)[i++];
                }
            s << ']';
            return s.str();
            }
    };

template< class S, class VD >
inline S& operator<<( S& s, const VectorTemplate<VD>& x )
    {
    s << x.toString();
    return s;
    }

template< class VD, class OP >
inline ctm::math::VectorTemplate< typename VD::vector_data_type > vectorMemberwiseUnaryOp(
        const ctm::math::VectorTemplate<VD>& a,
        OP op )
    {
    auto n = a.size();
    ctm::math::VectorTemplate< typename VD::vector_data_type > result( n );
    for( decltype(n) i=0; i<n; ++i )
        result[i] = op( a[i] );
    return result;
    }

template< class VD1, class VD2, class OP >
inline ctm::math::VectorTemplate< typename VD1::vector_data_type > vectorMemberwiseBinaryOp(
        const ctm::math::VectorTemplate<VD1>& a,
        const ctm::math::VectorTemplate<VD2>& b,
        OP op )
    {
    auto n = a.size();
    ASSERT( b.size() == n );
    ctm::math::VectorTemplate< typename VD1::vector_data_type > result( n );
    for( decltype(n) i=0; i<n; ++i )
        result[i] = op( a[i], b[i] );
    return result;
    }

template< class VD >
ctm::math::VectorTemplate< typename VD::vector_data_type > sqr( const ctm::math::VectorTemplate<VD>& a ) {
    return ctm::math::vectorMemberwiseUnaryOp( a, &sqr<double> );
    }

} // end namespace math
} // end namespace ctm

namespace std {

template< class VD >
inline ctm::math::VectorTemplate< typename VD::vector_data_type > max( const ctm::math::VectorTemplate<VD>& a, const ctm::math::VectorTemplate<VD>& b ) {
    return ctm::math::vectorMemberwiseBinaryOp(a, b, []( typename VD::value_type a, typename VD::value_type b ) { return max(a, b); } );
    }

template< class VD >
inline ctm::math::VectorTemplate< typename VD::vector_data_type > fabs( const ctm::math::VectorTemplate<VD>& a ) {
    return ctm::math::vectorMemberwiseUnaryOp( a, []( typename VD::value_type x ) { return fabs(x); } );
    }

template< class VD >
inline ctm::math::VectorTemplate< typename VD::vector_data_type > sqrt( const ctm::math::VectorTemplate<VD>& a ) {
    return ctm::math::vectorMemberwiseUnaryOp( a, []( typename VD::value_type x ) { return sqrt(x); } );
    }

} // end namespace std

#endif // _LA_VECTOR_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

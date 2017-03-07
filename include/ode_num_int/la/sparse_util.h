// sparse_util.h

#ifndef _LA_SPARSE_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _LA_SPARSE_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>

#include "../infra/cxx_assert.h"
#include "../infra/cxx_mock_ptr.h"
#include "../infra/cxx_exception.h"
#include "./vector_util.h"

namespace ctm {
namespace math {
namespace sparse {

namespace priv {

template< class This, class That >
void copyFrom( This& left, const That& right ) {
    left.clear();
    for( auto v : right )
        left.at( v.first ) = v.second;
    }
}

class SparseMatrixCommonTypes
    {
    public:
        typedef std::pair< unsigned int, unsigned int > Index;
    };

template< class ElementType >
class SparseMatrixTypes : public SparseMatrixCommonTypes
    {
    public:
        typedef ElementType element_type;
        typedef std::pair< const Index, element_type > Element;
    };

template< class ElementType >
class SparseMatrixData : public SparseMatrixTypes< ElementType >
    {
    public:
        typedef SparseMatrixData< ElementType > matrix_data_type;
        typedef SparseMatrixCommonTypes::Index Index;
        typedef ElementType element_type;
        typedef std::map< Index, element_type > Data;
        typedef typename Data::size_type size_type;
        typedef typename Data::iterator iterator;
        typedef typename Data::const_iterator const_iterator;

        SparseMatrixData() {}

        template< class That >
        SparseMatrixData( const That& that ) {
            priv::copyFrom( *this, that );
            }

        SparseMatrixData& operator=( const SparseMatrixData& that ) {
            if( this != &that )
                priv::copyFrom( *this, that );
            return *this;
            }

        template< class That >
        SparseMatrixData& operator=( const That& that ) {
            priv::copyFrom( *this, that );
            return *this;
            }

        element_type get( const Index& index ) const {
            auto it = m_data.find( index );
            return it == m_data.end() ?   0. :   it->second;
            }

        element_type get( unsigned int r, unsigned int c ) const {
            return get( Index( r, c ) );
            }

        bool hasIndex( const Index& index ) const {
            return m_data.find( index ) != m_data.end();
            }

        std::pair< element_type, bool > checkedGet( const Index& index ) const
            {
            auto it = m_data.find( index );
            return it == m_data.end() ?   std::make_pair( 0., false ) :   std::make_pair( it->second, true );
            }

        element_type operator[]( const Index& index ) const {
            return get( index );
            }

        element_type& at( const Index& index ) {
            return m_data[index];
            }

        element_type& at( unsigned int r, unsigned int c ) {
            return m_data[Index(r, c)];
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

        const_iterator rowBegin( unsigned int row ) const {
            return m_data.lower_bound( Index(row, 0) );
            }

        const_iterator rowEnd( unsigned int row ) const {
            return m_data.upper_bound( Index(row, ~0u) );
            }

        void resizeData( const Index& size )
            {
            for( auto it=m_data.begin(); it!=m_data.end(); ) {
                if( it->first.first < size.first   &&   it->first.second < size.second )
                    ++it;
                else
                    it = m_data.erase( it );
                }
            }

        void removeAt( const Index& index )
            {
            auto it = m_data.find( index );
            if( it != m_data.end() )
                m_data.erase( it );
            }

        void clear() {
            m_data.clear();
            }

        size_type count() const {
            return m_data.size();
            }

        bool empty() const {
            return m_data.empty();
            }

    private:
        Data m_data;
    };

template< class ElementType >
class SparseMatrixFastData : public SparseMatrixTypes< ElementType >
    {
    public:
        typedef SparseMatrixFastData< ElementType > matrix_data_type;
        typedef SparseMatrixCommonTypes::Index Index;
        typedef ElementType element_type;
        typedef std::pair< Index, element_type > data_element_type;
        typedef std::vector< data_element_type > Data;
        typedef typename Data::size_type size_type;
        typedef typename Data::iterator iterator;
        typedef typename Data::const_iterator const_iterator;

        SparseMatrixFastData() {}

        template< class That >
        explicit SparseMatrixFastData( const That& that ) {
            copyFrom( that );
            }

        matrix_data_type& operator=( const matrix_data_type& that ) {
            if( this != &that )
                copyFrom( that );
            return *this;
            }

        template< class That >
        matrix_data_type& operator=( const That& that ) {
            copyFrom( that );
            return *this;
            }

        element_type get( const Index& index ) const
            {
            auto it = std::lower_bound( begin(), end(), index, &matrix_data_type::compare );
            if( it == end()   ||   it->first != index )
                return element_type( 0. );
            else
                return it->second;
            }

        element_type get( unsigned int r, unsigned int c ) const {
            return get( Index( r, c ) );
            }

        bool hasIndex( const Index& index ) const
            {
            auto it = std::lower_bound( begin(), end(), index, &matrix_data_type::compare );
            return !( it == end()   ||   it->first != index );
            }

        std::pair< element_type, bool > checkedGet( const Index& index ) const
            {
            auto it = std::lower_bound( begin(), end(), index, &matrix_data_type::compare );
            if( it == end()   ||   it->first != index )
                return std::make_pair( element_type(0.), false );
            else
                return std::make_pair( it->second, true );
            }

        element_type operator[]( const Index& index ) const {
            return get( index );
            }

        element_type& at( const Index& index )
            {
            auto it = std::lower_bound( begin(), end(), index, &matrix_data_type::compare );
            if( it == end()   ||   it->first != index )
                throw cxx::exception( "Element is not found" );
            return it->second;
            }

        element_type& at( unsigned int r, unsigned int c ) {
            return at( Index( r, c ) );
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

        const_iterator rowBegin( unsigned int ) const {
            throw cxx::exception("Not implemented");
            }

        const_iterator rowEnd( unsigned int ) const {
            throw cxx::exception("Not implemented");
            }

        void resizeData( const Index& ) {
            throw cxx::exception("Not implemented");
            }

        void removeAt( const Index& ) {
            throw cxx::exception("Not implemented");
            }

        void clear() {
            m_data.clear();
            }

        size_type count() const {
            return m_data.size();
            }

        bool empty() const {
            return m_data.empty();
            }

    private:
        Data m_data;

        template< class That >
        void copyFrom( const That& that )
            {
            auto begin = that.begin();
            auto end = that.end();
            m_data.resize( std::distance( begin, end ) );
            std::copy( begin, end, m_data.begin() );
            }

        static bool compare( const data_element_type& l, const Index& r ) {
            return l.first < r;
            }
    };

namespace SparseMatrixProxy {

template< class D >
class Block : public SparseMatrixTypes< typename D::element_type >
    {
    public:
        typedef typename D::matrix_data_type matrix_data_type;
        typedef SparseMatrixCommonTypes::Index Index;
        typedef typename D::element_type element_type;
        typedef typename D::size_type size_type;
        Block( D& source, Index from, Index size ) :
            m_source( source ), m_from( from ), m_size( size )
            {
            }

        Block& operator=( const Block& that ) {
            if( this != &that )
                priv::copyFrom( *this, that );
            return *this;
            }

        template< class That >
        Block& operator=( const That& that ) {
            priv::copyFrom( *this, that );
            return *this;
            }

        element_type get( const Index& index ) const {
            return m_source.get( sourceIndex(index) );
            }

        element_type get( unsigned int r, unsigned int c ) const {
            return m_source.get( r, c );
            }

        element_type operator[]( const Index& index ) const {
            return get( index );
            }

        bool hasIndex( const Index& index ) const {
            return m_source.hasIndex( sourceIndex( index ) );
            }

        std::pair< element_type, bool > checkedGet( const Index& index ) const {
            return m_source.checkedGet( sourceIndex( index ) );
            }

        element_type& at( const Index& index ) {
            return m_source.at( sourceIndex(index) );
            }

        element_type at( unsigned int r, unsigned int c ) const {
            return at( Index( r, c ) );
            }

        template<class ActualElementType>
        using iterator_value_type = std::pair< const Index, std::reference_wrapper<ActualElementType> >;

        template< class EmbeddedIterator, class ActualElementType >
        class iterator_template : public std::iterator<
                std::forward_iterator_tag,
                iterator_value_type<ActualElementType>,
                std::ptrdiff_t,
                cxx::mock_ptr< iterator_value_type<ActualElementType> >,
                iterator_value_type<ActualElementType> >
            {
            friend class Block<D>;
            public:
                typedef iterator_template< EmbeddedIterator, ActualElementType > ThisClass;
                typedef iterator_value_type<ActualElementType> value_type;
                typedef cxx::mock_ptr< value_type > pointer;
                typedef value_type reference;
                const EmbeddedIterator& unwrap() const {
                    return m_it;
                    }
                reference operator*() const
                    {
                    typename EmbeddedIterator::reference v = *m_it;
                    return value_type( m_proxy.destinationIndex(v.first), v.second );
                    }
                pointer operator->() const {
                    return pointer( operator*() );
                    }

                template< class E2, class T2 >
                ThisClass& operator=( const iterator_template< E2, T2 >& that ) {
                    m_it = that.unwrap();
                    return *this;
                    }

                template< class E2, class T2 >
                bool operator==( const iterator_template< E2, T2 >& that ) const {
                    return m_it == that.unwrap();
                    }

                template< class E2, class T2 >
                bool operator!=( const iterator_template< E2, T2 >& that ) const {
                    return m_it != that.unwrap();
                    }

                iterator_template& operator++()
                    {
                    ++m_it;
                    incUntilValid();
                    return *this;
                    }

                template< class E2, class T2 >
                iterator_template( const iterator_template< E2, T2 >& that ) :
                    m_it( that.unwrap() ),
                    m_proxy( that.m_proxy )
                    {}
            private:
                private: EmbeddedIterator m_it;
                const Block<D>& m_proxy;

                iterator_template( const EmbeddedIterator& it, const Block<D>& proxy ) :
                    m_it( it ), m_proxy( proxy )
                    {
                    incUntilValid();
                    }

                bool isValid() const
                    {
                    ASSERT( m_it != m_proxy.m_source.end() );
                    auto sourceIndex = (*m_it).first;
                    return
                        sourceIndex.first >= m_proxy.m_from.first   &&
                        sourceIndex.first < m_proxy.m_from.first + m_proxy.m_size.first   &&
                        sourceIndex.second >= m_proxy.m_from.second   &&
                        sourceIndex.second < m_proxy.m_from.second + m_proxy.m_size.second;
                    }

                void incUntilValid() {
                    for( auto end=m_proxy.m_source.end(); m_it!=end && !isValid(); ++m_it ) {}
                    }

            };

        typedef iterator_template< typename D::iterator, element_type > iterator;
        typedef iterator_template< typename D::const_iterator, const element_type > const_iterator;

        iterator begin() {
            return iterator( m_source.begin(), *this );
            }
        const_iterator cbegin() const {
            return iterator( m_source.begin(), *this );
            }
        const_iterator begin() const {
            return iterator( m_source.begin(), *this );
            }

        iterator end() {
            return iterator( m_source.end(), *this );
            }
        const_iterator cend() const {
            return const_iterator( m_source.end(), *this );
            }
        const_iterator end() const {
            return const_iterator( m_source.end(), *this );
            }

        const_iterator rowBegin( unsigned int row ) const
            {
            ASSERT( row < m_size.first );
            return const_iterator( m_source.rowBegin( row + m_from.first ), *this );
            }

        const_iterator rowEnd( unsigned int row ) const {
            ASSERT( row < m_size.first );
            return const_iterator( m_source.rowEnd( row + m_from.first ), *this );
            }

        void removeAt( const Index& index ) {
            m_source.removeAt( sourceIndex( index ) );
            }

        void clear()
            {
            for( auto it=begin(), e=end(); it!=e; ) {
                auto it4rm = it;
                ++it;
                removeAt( it4rm->first );
                }
            }

        size_type count() const
            {
            unsigned int result = 0;
            for( auto it = begin(); it!=end(); ++it )
                ++result;
            return result;
            }

        bool empty() const {
            return count() == 0;
            }

    private:
        D& m_source;
        Index m_from;
        Index m_size;
        void assertIndexValid( const Index& index ) const
            {
            ASSERT( index.first < m_size.first );
            ASSERT( index.second < m_size.second );
            }

        Index sourceIndex( const Index& index ) const
            {
            assertIndexValid( index );
            return Index( index.first + m_from.first, index.second + m_from.second );
            }

        Index destinationIndex( const Index& index ) const
            {
            auto result = Index( index.first - m_from.first, index.second - m_from.second );
            assertIndexValid( result );
            return result;
            }
    };

template< class D >
class Transpose : public SparseMatrixTypes< typename D::element_type >
    {
    public:
        typedef typename D::matrix_data_type matrix_data_type;
        typedef SparseMatrixCommonTypes::Index Index;
        typedef typename D::element_type element_type;
        typedef typename D::size_type size_type;
        explicit Transpose( D& source ) :
            m_source( source )
            {
            }

        element_type get( const Index& index ) const {
            return m_source.get( transposedIndex(index) );
            }

        element_type get( unsigned int r, unsigned int c ) const {
            return m_source.get( r, c );
            }

        element_type operator[]( const Index& index ) const {
            return get( index );
            }

        bool hasIndex( const Index& index ) const {
            return m_source.hasIndex( transposedIndex( index ) );
            }

        std::pair< element_type, bool > checkedGet( const Index& index ) const {
            return m_source.checkedGet( transposedIndex( index ) );
            }

        element_type& at( const Index& index ) {
            return m_source.at( transposedIndex(index) );
            }

        element_type at( unsigned int r, unsigned int c ) const {
            return at( Index( r, c ) );
            }

        template<class ActualElementType>
        using iterator_value_type = std::pair< const Index, std::reference_wrapper<ActualElementType> >;

        template< class EmbeddedIterator, class ActualElementType >
        class iterator_template : public std::iterator<
                std::forward_iterator_tag,
                iterator_value_type<ActualElementType>,
                std::ptrdiff_t,
                cxx::mock_ptr< iterator_value_type<ActualElementType> >,
                iterator_value_type<ActualElementType> >
            {
            friend class Transpose<D>;
            public:
                typedef iterator_template< EmbeddedIterator, ActualElementType > ThisClass;
                typedef iterator_value_type<ActualElementType> value_type;
                typedef cxx::mock_ptr< value_type > pointer;
                typedef value_type reference;
                const EmbeddedIterator& unwrap() const {
                    return m_it;
                    }
                reference operator*() const
                    {
                    typename EmbeddedIterator::reference v = *m_it;
                    return value_type( transposedIndex(v.first), v.second );
                    }
                pointer operator->() const {
                    return pointer( operator*() );
                    }

                template< class E2, class T2 >
                ThisClass& operator=( const iterator_template< E2, T2 >& that ) {
                    m_it = that.unwrap();
                    return *this;
                    }

                template< class E2, class T2 >
                bool operator==( const iterator_template< E2, T2 >& that ) const {
                    return m_it == that.unwrap();
                    }

                template< class E2, class T2 >
                bool operator!=( const iterator_template< E2, T2 >& that ) const {
                    return m_it != that.unwrap();
                    }

                iterator_template& operator++()
                    {
                    ++m_it;
                    return *this;
                    }

                template< class E2, class T2 >
                iterator_template( const iterator_template< E2, T2 >& that ) :
                    m_it( that.unwrap() )
                    {}
            private:
                private: EmbeddedIterator m_it;

                iterator_template( const EmbeddedIterator& it ) :
                    m_it( it )
                    {
                    }
            };

        typedef iterator_template< typename D::iterator, element_type > iterator;
        typedef iterator_template< typename D::const_iterator, const element_type > const_iterator;

        iterator begin() {
            return iterator( m_source.begin() );
            }
        const_iterator cbegin() const {
            return iterator( m_source.begin() );
            }
        const_iterator begin() const {
            return iterator( m_source.begin() );
            }

        iterator end() {
            return iterator( m_source.end() );
            }
        const_iterator cend() const {
            return const_iterator( m_source.end() );
            }
        const_iterator end() const {
            return const_iterator( m_source.end() );
            }

        void removeAt( const Index& index ) {
            m_source.removeAt( transposedIndex( index ) );
            }

        void clear()
            {
            for( auto it=begin(), e=end(); it!=e; ) {
                auto it4rm = it;
                ++it;
                removeAt( it4rm->first );
                }
            }

        size_type count() const {
            return m_source.count();
            }

        bool empty() const {
            return m_source.empty();
            }

    private:
        D& m_source;

        static Index transposedIndex( const Index& index ) {
            return Index( index.second, index.first );
            }
    };

template< class D >
class Scale : public SparseMatrixTypes< typename D::element_type >
    {
    public:
        typedef typename D::matrix_data_type matrix_data_type;
        typedef SparseMatrixCommonTypes::Index Index;
        typedef typename D::element_type element_type;
        typedef typename D::size_type size_type;
        Scale( const D& source, element_type scaleFactor ) :
            m_source( source ), m_scaleFactor( scaleFactor )
            {}

        template< class EmbeddedIterator >
        class iterator_template : public std::iterator<
                std::forward_iterator_tag,
                typename Scale<D>::Element,
                std::ptrdiff_t,
                cxx::mock_ptr< typename Scale<D>::Element >,
                typename Scale<D>::Element >
            {
            friend class Scale<D>;
            public:
                typedef iterator_template< EmbeddedIterator > ThisClass;
                typedef typename Scale<D>::Element value_type;
                typedef cxx::mock_ptr< value_type > pointer;
                typedef value_type reference;
                const EmbeddedIterator& unwrap() const {
                    return m_it;
                    }
                value_type operator*() const {
                    return value_type( m_it->first, m_it->second * m_scaleFactor );
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
                element_type m_scaleFactor;

                iterator_template( const EmbeddedIterator& it, element_type scaleFactor ) :
                    m_it( it ), m_scaleFactor( scaleFactor )
                    {
                    }
            };

        typedef iterator_template< typename D::const_iterator > const_iterator;
        typedef iterator_template< typename D::const_iterator > iterator;

        element_type get( const Index& index ) const {
            return m_source.get( index ) * m_scaleFactor;
            }

        element_type get( unsigned int r, unsigned int c ) const {
            return m_source.get( r, c ) * m_scaleFactor;
            }

        element_type operator[]( const Index& index ) const {
            return get( index );
            }

        bool hasIndex( const Index& index ) const {
            return m_source.hasIndex( index );
            }

        std::pair< element_type, bool > checkedGet( const Index& index ) const {
            auto result = m_source.checkedGet( index );
            result.first *= m_scaleFactor;
            return result;
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

        const_iterator rowBegin( unsigned int row ) const {
            return const_iterator( m_source.rowBegin( row ), m_scaleFactor );
            }

        const_iterator rowEnd( unsigned int row ) const {
            return const_iterator( m_source.rowEnd( row ), m_scaleFactor );
            }

        size_type count() const {
            return m_source.count();
            }

        bool empty() const {
            return m_source.empty();
            }
    private:
        const D& m_source;
        element_type m_scaleFactor;
    };

} // namespace SparseMatrixProxy

template< class D > class SparseMatrixTemplate;

template< class ElementType >
using SparseMatrix = SparseMatrixTemplate< SparseMatrixData< ElementType > >;

template< class D >
class SparseMatrixTemplate : public D
    {
    public:
        typedef typename D::matrix_data_type matrix_data_type;
        typedef SparseMatrixTemplate<D> ThisClass;
        typedef typename D::element_type element_type;
        typedef typename D::Index Index;

        SparseMatrixTemplate() {}
        explicit SparseMatrixTemplate( const Index& size ) : m_size(size) {}
        SparseMatrixTemplate( unsigned int rows, unsigned int columns ) : m_size(rows, columns) {}
        explicit SparseMatrixTemplate( const D& data ) :
            D( data ), m_size( sizeFromData() ) {}
        SparseMatrixTemplate( const D& data, const Index& size ) :
            D( data ), m_size( size ) {}

        template< class ThatD >
        SparseMatrixTemplate( const SparseMatrixTemplate< ThatD >& that ) : m_size( that.size() )
            {
            for( auto v : that )
                this->at( v.first ) = v.second;
            }

        template< class ThatD >
        ThisClass& operator=( const SparseMatrixTemplate< ThatD >& that ) {
            return doAssign( that );
            }

        ThisClass& operator=( const ThisClass& that )
            {
            if( &that == this )
                return *this;
            else
                return doAssign( that );
            }

        Index size() const {
            return m_size;
            }

        void resize( const Index& size )
            {
            if( m_size != size ) {
                m_size = size;
                this->resizeData( m_size );
                }
            }

        void resize( unsigned int rows, unsigned int columns ) {
            resize( Index(rows, columns) );
            }

        Index sizeFromData() const
            {
            Index result;
            for( auto v : *this ) {
                result.first = std::max( result.first, v.first.first+1 );
                result.second = std::max( result.second, v.first.second+1 );
                }
            return result;
            }

        void resizeFromData() {
            resize( sizeFromData() );
            }

        ThisClass& makeZero()
            {
            for( auto& v : *this )
                v.second = element_type(0);
            return *this;
            }

        ThisClass& addScaledIdentity( element_type scaleFactor )
            {
            ASSERT( m_size.first == m_size.second );
            for( unsigned int i=0; i<m_size.first; ++i )
                this->at( Index(i,i) ) += scaleFactor;
            return *this;
            }

        ThisClass& addIdentity() {
            return addScaledIdentity( 1 );
            }

        template< class ThatD >
        ThisClass& operator+=( const SparseMatrixTemplate< ThatD >& that )
            {
            ASSERT( m_size == that.size() );
            for( auto v : that )
                this->at( v.first ) += v.second;
            return *this;
            }

        template< class ThatD >
        ThisClass& operator-=( const SparseMatrixTemplate< ThatD >& that )
            {
            ASSERT( m_size == that.size() );
            for( auto v : that )
                this->at( v.first ) -= v.second;
            return *this;
            }

        template< class ThatD >
        SparseMatrix<element_type> operator*( const SparseMatrixTemplate< ThatD >& that ) const
            {
            auto thatSize = that.size();
            ASSERT( m_size.second == thatSize.first );
            SparseMatrix<element_type> result( m_size.first, thatSize.second );
            for( unsigned int c=0; c<thatSize.second; ++c )
                for( unsigned int r=0; r<m_size.first; ++r ) {
                    element_type product = 0;
                    for( auto it=this->rowBegin(r), end=this->rowEnd(r); it!=end; ++it ) {
                        auto r = that.checkedGet( Index( it->first.second, c ) );
                        if( r.second )
                            product += it->second * r.first;
                        }
                    if( product != 0 )
                        result.at( r, c ) = product;
                    }
            return result;
            }

        template< class ThatD >
        ThisClass& operator*=( const SparseMatrixTemplate< ThatD >& that )
            {
            ASSERT( m_size.first == m_size.second   &&   m_size == that.size() );
            *this = *this * that;
            return *this;
            }

        template< class It1, class It2 >
        void mulVectRight( It1 vs, It2 vd ) const {
            std::fill( vd, vd+m_size.first, 0 );
            for( auto v : *this )
                *( vd + v.first.first ) += v.second * *( vs + v.first.second );
            }

        template< class It1, class It2 >
        void mulVectLeft( It1 vs, It2 vd ) const {
            std::fill( vd, vd+m_size.second, 0 );
            for( auto v : *this )
                *( vd + v.first.second ) += v.second * *( vs + v.first.first );
            }

        SparseMatrixTemplate< SparseMatrixProxy::Block< D > > block( const Index& from, const Index& size )
            {
            ASSERT( from.first + size.first <= m_size.first   &&   from.second + size.second <= m_size.second );
            return SparseMatrixTemplate< SparseMatrixProxy::Block< D > >(
                        SparseMatrixProxy::Block< D >( *this, from, size ), size );
            }

        SparseMatrixTemplate< SparseMatrixProxy::Block< D > > block(
                unsigned int fromRow, unsigned int fromColumn,
                unsigned int rowCount, unsigned int columnCount )
            {
            return block( Index(fromRow, fromColumn), Index(rowCount, columnCount) );
            }

        SparseMatrixTemplate< SparseMatrixProxy::Scale< D > > scaled( element_type scaleFactor )
            {
            return SparseMatrixTemplate< SparseMatrixProxy::Scale< D > >(
                        SparseMatrixProxy::Scale< D >( *this, scaleFactor ),
                        m_size );
            }

        SparseMatrixTemplate< SparseMatrixProxy::Transpose< D > > transposed()
            {
            return SparseMatrixTemplate< SparseMatrixProxy::Transpose< D > >(
                        SparseMatrixProxy::Transpose< D >( *this ),
                        m_size );
            }

        SparseMatrixTemplate<matrix_data_type> clone() const {
            return SparseMatrixTemplate<matrix_data_type>( *this );
            }

        element_type infNorm() const
            {
            element_type result = element_type ();
            for( const auto& v : *this )
                result = std::max( result, std::fabs( v.second ) );
            return result;
            }

        element_type oneNorm() const
            {
            element_type result = element_type ();
            for( const auto& v : *this )
                result += std::fabs( v.second );
            return result;
            }

        element_type euclideanNormSquare() const
            {
            element_type result = element_type ();
            for( const auto& v : *this )
                result += v.second * v.second;
            return result;
            }

        element_type euclideanNorm() const {
            return std::sqrt( euclideanNormSquare() );
            }

        static SparseMatrixTemplate<matrix_data_type> identity( unsigned int size )
            {
            SparseMatrixTemplate<matrix_data_type> result( size, size );
            result.addIdentity();
            return result;
            }

        static SparseMatrixTemplate<matrix_data_type> scaledIdentity( unsigned int size, element_type scaleFactor )
            {
            SparseMatrixTemplate<matrix_data_type> result( size, size );
            result.addScaledIdentity( scaleFactor );
            return result;
            }

    private:
        Index m_size;

        template< class ThatD >
        ThisClass& doAssign( const SparseMatrixTemplate< ThatD >& that )
            {
            m_size = that.size();
            D& d = *this;
            d = that;
            return *this;
            }
    };

typedef SparseMatrix<double> SparseMatrixD;

template< class DM, class DV >
inline VectorTemplate<typename DV::vector_data_type> operator*( const SparseMatrixTemplate<DM>& m, const VectorTemplate<DV>& v )
    {
    ASSERT( v.size() == m.size().second );
    VectorTemplate<typename DV::vector_data_type> result( m.size().first );
    m.mulVectRight( v.begin(), result.begin() );
    return result;
    }

template< class DM, class DV >
inline VectorTemplate<typename DV::vector_data_type> operator*( const VectorTemplate<DV>& v, const SparseMatrixTemplate<DM>& m )
    {
    ASSERT( v.size() == m.size().first );
    VectorTemplate<typename DV::vector_data_type> result( m.size().first );
    m.mulVectLeft( v.begin(), result.begin() );
    return result;
    }

template< class S, class D >
inline S& operator<<( S& stream, const SparseMatrixTemplate<D>& m )
    {
    using namespace std;
    vector<string> elements( m.size().first * m.size().second, "0" );
    for( auto v : m ) {
        auto i = v.first.second + v.first.first*m.size().second;
        ostringstream oss;
        oss << v.second;
        elements[i] = oss.str();
        }
    vector<string::size_type> colWidth( m.size().second, 0 );
    for( unsigned int row=0; row<m.size().first; ++row )
        for( unsigned int col=0; col<m.size().second; ++col ) {
            auto i = col + row*m.size().second;
            auto& w = colWidth[col];
            w = max( w, elements[i].size() + 1 );
            }
    for( unsigned int row=0; row<m.size().first; ++row ) {
        for( unsigned int col=0; col<m.size().second; ++col ) {
            auto i = col + row*m.size().second;
            auto e = elements[i];
            stream << e << string(colWidth[col]-e.size(), ' ');
            }
        stream << endl;
        }
    return stream;
    }

template< class S, class D >
inline void printScilabMatrix( S& stream, const SparseMatrixTemplate<D>& m, const std::string& name )
    {
    using namespace std;
    stream << name << " = zeros(" << m.size().first << ", " << m.size().second << ");" << endl;
    for (auto v : m)
        stream << name << "(" << v.first.first+1 << ", " << v.first.second+1 << ") = " << v.second << ";" << endl;
    }

} // end namespace sparse
} // end namespace math
} // end namespace ctm

#endif // _LA_SPARSE_UTIL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// observers.h

#ifndef _CXX_OBSERVERS_H_C2F4FC47_8A32_4855_BB43_2AD22EBE0C7E_
#define _CXX_OBSERVERS_H_C2F4FC47_8A32_4855_BB43_2AD22EBE0C7E_

#include <functional>
#include <map>
#include "./cxx_assert.h"

namespace ctm {
namespace cxx {

template< class T >
class IdentifiedElements
    {
    public:
        typedef typename std::map< int, T >::iterator iterator;
        typedef typename std::map< int, T >::const_iterator const_iterator;

        iterator begin() { return m_elements.begin(); }
        const_iterator begin() const { return m_elements.begin(); }
        iterator end() { return m_elements.end(); }
        const_iterator end() const { return m_elements.end(); }

        int add( T element )
            {
            int id = newId();
            m_elements[id] = element;
            return id;
            }

        void remove( int id )
            {
            auto it = m_elements.find( id );
            if( it == m_elements.end() ) {
                ASSERT( false );
                }
            else
                m_elements.erase( it );
            }

        void clear() {
            m_elements.clear();
            }

        bool empty() const {
            return m_elements.empty();
            }

    private:
        std::map< int, T > m_elements;
        int newId() const
            {
            if( m_elements.empty() )
                return 1;
            else {
                int result = m_elements.rbegin()->first + 1;
                ASSERT( m_elements.find( result ) == m_elements.end() );
                return result;
                }
            }
    };

template< class T >
class ScopedIdentifiedElement
    {
    public:
        ScopedIdentifiedElement( IdentifiedElements<T>& elements, T element ) :
            m_elements(elements),
            m_id( elements.add( element ) )
            {
            }

        ~ScopedIdentifiedElement() {
            finish();
            }

        ScopedIdentifiedElement( const ScopedIdentifiedElement<T>& ) = delete;
        ScopedIdentifiedElement<T>& operator=( const ScopedIdentifiedElement<T>& ) = delete;

        ScopedIdentifiedElement( ScopedIdentifiedElement<T>&& that ) : m_elements(that.m_elements), m_id(that.m_id) {
            that.m_id = -1;
            }

        ScopedIdentifiedElement<T>& operator=( ScopedIdentifiedElement<T>&& ) = delete;

        void finish() {
            if( m_id != -1 ) {
                m_elements.remove( m_id );
                m_id = -1;
                }
            }

    private:
        IdentifiedElements<T>& m_elements;
        int m_id;
    };

template< class T1, class T2 >
ScopedIdentifiedElement<T1> makeScopedIdentifiedElement( IdentifiedElements<T1>& elements, T2 element ) {
    return ScopedIdentifiedElement<T1>( elements, element );
    }


template< class ... args >
class Observers : public IdentifiedElements< std::function<void(args ...)> >
    {
    public:
        typedef std::function<void(args ...)> cb_type;

        void operator()( args... a ) const {
            for( const auto& idAndObserver : *this )
                idAndObserver.second( a... );
            }
    };

template< class ... args >
class TerminatingObservers : public IdentifiedElements< std::function<bool(args ...)> >
    {
    public:
        typedef std::function<bool(args ...)> cb_type;

        bool operator()( args... a ) const {
            bool result = false;
            for( const auto& idAndObserver : *this )
                result = idAndObserver.second( a... ) || result;
            return result;
            }
    };

template< class ... args >
inline void forwardObservers( cxx::Observers<args...>& caller, cxx::Observers<args...>& callee ) {
    caller.add( [&callee]( args ... a ) {
        callee( a... );
        } );
    }

template< class ... args >
inline void forwardObservers( cxx::TerminatingObservers<args...>& caller, cxx::TerminatingObservers<args...>& callee ) {
    caller.add( [&callee]( args ... a ) -> bool {
        return callee( a... );
        } );
    }

} // end namespace cxx
} // end namespace ctm

#endif // _CXX_OBSERVERS_H_C2F4FC47_8A32_4855_BB43_2AD22EBE0C7E_

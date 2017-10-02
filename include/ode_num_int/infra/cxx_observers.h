// cxx_observers.h

/// \file
/// \brief Declares ctm::cxx::Observers, ctm::cxx::TerminatingObservers, and related template classes and functions.

#ifndef _INFRA_CXX_OBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_CXX_OBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include <functional>
#include <map>
#include "./cxx_assert.h"

namespace ctm {
namespace cxx {

/// \brief Sequence of elements automatically identified by integer numbers.
template< class T >
class IdentifiedElements
    {
    public:
        /// \brief Iterator type.
        typedef typename std::map< int, T >::iterator iterator;

        /// \brief Constant iterator type
        typedef typename std::map< int, T >::const_iterator const_iterator;

        /// \brief Returns iterator to the beginning of the sequence.
        iterator begin() { return m_elements.begin(); }

        /// \brief Returns constant iterator to the beginning of the sequence.
        const_iterator begin() const { return m_elements.begin(); }

        /// \brief Returns iterator to the end of the sequence.
        iterator end() { return m_elements.end(); }

        /// \brief Returns constant iterator to the end of the sequence.
        const_iterator end() const { return m_elements.end(); }

        /// \brief Add element at the end of the sequence.
        /// \param element Element to add.
        /// \return Identifier of the element added.
        int add( T element )
            {
            int id = newId();
            m_elements[id] = element;
            return id;
            }

        /// \brief Removes element with specified identifier.
        /// \param id Identifier of the element to be removed.
        /// \note If \a id does not correspond to an existing element, nothing is done,
        /// but an assertion fails in debug builds.
        void remove( int id )
            {
            auto it = m_elements.find( id );
            if( it == m_elements.end() ) {
                ASSERT( false );
                }
            else
                m_elements.erase( it );
            }

        /// \brief Removes all elements from the sequence.
        void clear() {
            m_elements.clear();
            }

        /// \brief Returns true if the sequence is empty and false otherwise.
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

/// \brief Implements scoped addition/deletion of an element to/from sequence.
///
/// When you need some element to be present in a sequence of identified elements
/// till the end of the block, and then removed, declare a local variable of this class;
/// pass the sequence and the element to add in the constructor. The element will be
/// added to the sequence in the constructor and removed from the sequence in the
/// destructor. It is also possible to remove the element earlier by calling
/// ScopedIdentifiedElement::finish().
///
/// Example:
/// \code
/// void f(IdentifiedElements<int>& elements)
/// {
///     ScopedIdentifiedElement<int> s(elements, 42);
///
///     // elements contain 42 till the end of the block,
///     // and then removed by s's destructor.
/// }
/// \endcode
template< class T >
class ScopedIdentifiedElement
    {
    public:
        /// \brief Constructor.
        /// \param elements Sequence of identified elements to be modified.
        /// \param element Element to be added to the sequence.
        ///
        /// Adds \a element to \a elements, such that the element is in the sequence
        /// till this instance is destroyed or finish() is called.
        ScopedIdentifiedElement( IdentifiedElements<T>& elements, T element ) :
            m_elements(elements),
            m_id( elements.add( element ) )
            {
            }

        /// \brief Destructor.
        ///
        /// Removes element from the sequence (both the element and the sequence
        /// are passed in the constructor), unless finish() is already called.
        ~ScopedIdentifiedElement() {
            finish();
            }

        ScopedIdentifiedElement( const ScopedIdentifiedElement<T>& ) = delete;
        ScopedIdentifiedElement<T>& operator=( const ScopedIdentifiedElement<T>& ) = delete;

        ScopedIdentifiedElement( ScopedIdentifiedElement<T>&& that ) : m_elements(that.m_elements), m_id(that.m_id) {
            that.m_id = -1;
            }

        ScopedIdentifiedElement<T>& operator=( ScopedIdentifiedElement<T>&& ) = delete;

        /// \brief Does what destructor would normally do.
        ///
        /// Removes element from the sequence (both the element and the sequence
        /// are passed in the constructor). If already called before, does nothing.
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

/// \brief Generates ScopedIdentifiedElement instance.
///
/// The instance returned can be used to initialize a local variable, thus making it
/// possible to use the auto keyword, e.g.
/// \code
/// void f(IdentifiedElements<int>& elements)
/// {
///     auto s = makeScopedIdentifiedElement(elements, 42);
///     // ...
/// }
/// \endcode
template< class T1, class T2 >
ScopedIdentifiedElement<T1> makeScopedIdentifiedElement( IdentifiedElements<T1>& elements, T2 element ) {
    return ScopedIdentifiedElement<T1>( elements, element );
    }

/// \brief Sequence of callbacks that can be called at once.
///
/// This template class can be used in a similar way boost::signals2::signal can
/// (see <a href="http://www.boost.org/doc/libs/1_65_1/doc/html/signals2.html">Boost.Signals2</a>).
/// It provides a simple signal-slot mechanism.
///
/// Example:
/// \code
/// struct Foo {
///     Observers<int, double> barObservers;
///     void bar() {
///         // ...
///         barObservers(1, 2.3);
///     }
/// };
///
/// void baz() {
///     Foo foo;
///     foo.barObservers.add([](int a, double x) {
///         cout << "(" << a << ", " << x << ")\n";
///     });
///     foo.bar(); // prints (1, 2.3)
/// }
/// \endcode
/// \sa ctm::cxx::TerminatingObservers.
template< class ... args >
class Observers : public IdentifiedElements< std::function<void(args ...)> >
    {
    public:
        /// \brief Callback type
        typedef std::function<void(args ...)> cb_type;

        /// \brief Invokes all callbacks in the sequence, from first to last.
        void operator()( args... a ) const {
            for( const auto& idAndObserver : *this )
                idAndObserver.second( a... );
            }
    };

/// \brief Sequence of callbacks that return a boolean value and that can be called at once.
///
/// This template class is similar to ctm::cxx::Observers, but callbacks
/// return a boolean value; that can be used, for example, to let the callbacks
/// control instruction flow.
/// \sa ctm::cxx::Observers.
template< class ... args >
class TerminatingObservers : public IdentifiedElements< std::function<bool(args ...)> >
    {
    public:
        /// \brief Callback type
        typedef std::function<bool(args ...)> cb_type;

        /// \brief Invokes all callbacks in the sequence, from first to last.
        /// \return If each callback returns false, the method returns false;
        /// if at least one callback returns true, the method returns true. In
        /// other words, the result of the method is logical OR between the results
        /// of values returned by all callbacks. If there are no callbacks at all,
        /// the method returns false.
        bool operator()( args... a ) const {
            bool result = false;
            for( const auto& idAndObserver : *this )
                result = idAndObserver.second( a... ) || result;
            return result;
            }
    };

/// \brief Adds an observer to \a caller that invokes all observers from \a callee.
/// \param caller Sequence of callbacks where the new observer is to be added.
/// \param callee Sequence of callbacks to be invoked by the new observer.
/// \return Identifier of the new observer in the \a caller sequence.
template< class ... args >
inline int forwardObservers( cxx::Observers<args...>& caller, cxx::Observers<args...>& callee ) {
    return caller.add( [&callee]( args ... a ) {
        callee( a... );
        } );
    }

/// \brief Adds an observer to \a caller that invokes all observers from \a callee.
/// \param caller Sequence of callbacks where the new observer is to be added.
/// \param callee Sequence of callbacks to be invoked by the new observer.
/// \return Identifier of the new observer in the \a caller sequence.
template< class ... args >
inline int forwardObservers( cxx::TerminatingObservers<args...>& caller, cxx::TerminatingObservers<args...>& callee ) {
    return caller.add( [&callee]( args ... a ) -> bool {
        return callee( a... );
        } );
    }

} // end namespace cxx
} // end namespace ctm

#endif // _INFRA_CXX_OBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

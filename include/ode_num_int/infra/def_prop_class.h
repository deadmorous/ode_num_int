// def_prop_class.h

/// \file
/// \brief Macros to declare property holder classes.

#ifndef _INFRA_DEF_PROP_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_DEF_PROP_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./cxx_observers.h"

namespace ctm {
namespace def_prop_class_util {

// See http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

template< class T >
struct function_traits : public function_traits<decltype(&T::operator())> {};

template< class C, class R, class... Args >
struct function_traits<R(C::*)(Args...) const> {
    typedef R return_type;
    typedef std::tuple<Args...> args_type;
    static const size_t arity = sizeof...(Args);
    typedef std::function<R(Args...)> std_function_type;
};

} // end namespace def_prop_class_util
} // end namespace ctm

/// \brief Declares a class with a single private field and public getter and setter methods for it.
/// \param ClassName The name of the class.
/// \param PropType The type of the private field.
/// \param PassedPropType The type to use for setter parameter and const getter return value.
/// \param propGetter The name of the getter method.
/// \param propSetter The name of the setter method.
/// \note The field can be initialized by passing the initializer value to the constructor.
/// \note The class also has the const getter, named \a propGetter followed by \c constRef.
/// \note The typical use of this macro is as follows.
/// - Declare a class using this macro.
/// - Inherit the class by a class that needs the corresponding property.
/// \sa CTM_DECL_SIMPLE_CLASS_FIELD, CTM_DEF_NOTIFIED_PROP_CLASS, CTM_DEF_PROP_REF_CLASS.
#define CTM_DEF_PROP_CLASS(ClassName, PropType, PassedPropType, propGetter, propSetter) \
    class ClassName { \
    public: \
        explicit ClassName(PassedPropType value) : m_value(value) {} \
        ClassName() : m_value() {} \
        PassedPropType propGetter() const { \
            return m_value; \
        } \
        void propSetter(PassedPropType value) { \
            m_value = value; \
        } \
        PassedPropType propGetter##ConstRef() const { \
            return m_value; \
        } \
    private: \
        PropType m_value; \
    };



/// \brief Declares a class with a single private field, public getter and setter methods, and observers for value modification.
/// \param ClassName The name of the class.
/// \param PropType The type of the private field.
/// \param PassedPropType The type to use for setter parameter and,
/// const getter return value, and modification observer parameter.
/// \param propGetter The name of the getter method.
/// \param propSetter The name of the setter method.
/// \param addOnChangeObserver The name of the method to add modification observer.
/// \param removeOnChangeObserver The name of the method to remove modification observer.
/// \note The field can be initialized by passing the initializer value to the constructor.
/// \note The class also has the const getter, named \a propGetter followed by \c constRef.
/// \note Two kinds of observer callbacks are accepted: without arguments and with one argument of type \a PassedPropType.
/// If compiler fails to find the appropriate overload, one can use methods named \a addOnChangeObserver followed by
/// \c _0 (to add observers without arguments) or \a addOnChangeObserver followed by \c _1
/// (to add observers with one argument).
/// \note The typical use of this macro is as follows.
/// - Declare a class using this macro.
/// - Inherit the class by a class that needs the corresponding property.
/// .
///
/// Example:
/// \code
/// class Foo { ... };
/// CTM_DEF_NOTIFIED_PROP_CLASS(
///     WithFoo, Foo, const Foo&, foo, setFoo, onChangeFoo, offChangeFoo)
///
/// class Bar : public WithFoo {
/// public:
///     Bar() {
///         onChangeFoo([](){ cout << "foo has changed\n"; });
///     }
/// };
///
/// void baz() {
///     Bar bar;
///     bar.setFoo(Foo());    // Prints "foo has changed"
/// }
/// \endcode
/// \sa CTM_DECL_SIMPLE_CLASS_FIELD, CTM_DEF_PROP_CLASS, CTM_DEF_PROP_REF_CLASS.
#define CTM_DEF_NOTIFIED_PROP_CLASS(ClassName, PropType, PassedPropType, propGetter, propSetter, addOnChangeObserver, removeOnChangeObserver) \
    class ClassName { \
    public: \
        explicit ClassName(PassedPropType value) : m_value(value) {} \
        ClassName() : m_value() {} \
        PassedPropType propGetter() const { \
            return m_value; \
        } \
        void propSetter(PassedPropType value) { \
            m_value = value; \
            m_onChange( m_value ); \
        } \
        PassedPropType propGetter##ConstRef() const { \
            return m_value; \
        } \
        int addOnChangeObserver##_0( const std::function<void()>& observer ) { \
            return addOnChangeObserverPriv( observer ); \
        } \
        int addOnChangeObserver##_1( const std::function<void(PassedPropType)>& observer ) { \
            return addOnChangeObserverPriv( observer ); \
        } \
        template< class F > \
        int addOnChangeObserver( F observer ) { \
            return addOnChangeObserverPriv( typename ctm::def_prop_class_util::function_traits<F>::std_function_type( observer ) ); \
        } \
        void removeOnChangeObserver( int observerId ) { \
            m_onChange.remove( observerId ); \
        } \
    private: \
        PropType m_value; \
        ctm::cxx::Observers< PassedPropType > m_onChange; \
        int addOnChangeObserverPriv( const std::function<void()>& observer ) { \
            return m_onChange.add( [observer](PassedPropType) { observer(); } ); \
        } \
        int addOnChangeObserverPriv( const std::function<void(PassedPropType)>& observer ) { \
            return m_onChange.add( observer ); \
        } \
    };

/// \brief Declares class holding a reference to a value in a private field, with accessor methods.
/// \param ClassName The name of the class.
/// \param PropType The type of the private field (logically, some kind of reference, e.g., std::shared_ptr).
/// \param propRefAccessor The name of non-constant accessor method returning \a PropType&.
/// \param constPropRefAccessor The name of constant accessor method returning const \a PropType&.
/// \note The typical use of this macro is as follows.
/// - Declare a class using this macro.
/// - Inherit the class by a class that needs the corresponding property.
/// \sa CTM_DECL_SIMPLE_CLASS_FIELD, CTM_DEF_PROP_CLASS, CTM_DEF_NOTIFIED_PROP_CLASS.
#define CTM_DEF_PROP_REF_CLASS(ClassName, PropType, propRefAccessor, constPropRefAccessor) \
    class ClassName { \
    public: \
        ClassName() : m_value() {} \
        explicit ClassName(const PropType& value) : m_value(value) {} \
        PropType& propRefAccessor() { \
            return m_value; \
        } \
        const PropType& propRefAccessor() const { \
            return m_value; \
        } \
        const PropType& constPropRefAccessor() const { \
            return m_value; \
        } \
    private: \
        PropType m_value; \
    };

#endif // _INFRA_DEF_PROP_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

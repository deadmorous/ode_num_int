// def_prop_class.h

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

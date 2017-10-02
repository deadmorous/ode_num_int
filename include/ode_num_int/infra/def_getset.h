// def_getset.h

/// \file
/// \brief Macros to declare private fields with public getter and setter methods.

#ifndef _INFRA_DEF_GETSET_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_DEF_GETSET_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

/// \brief Declares the ThisClass type to be an alias of \a ClassName.
/// \note Use this macro within a class declaration.
#define CTM_DECL_THIS_CLASS_ALIAS( ClassName ) using ThisClass = ClassName;

/// \brief Declares private field with public getter and setter methods.
/// \param type The type of the field.
/// \param name The name of the getter method.
/// \param setterName The name of the setter method.
/// \param initializer The value of the field initializer.
/// \note Use CTM_DECL_THIS_CLASS_ALIAS before using this macro.
/// \note Use this macro within a class declaration.
/// \sa CTM_DEF_PROP_CLASS, CTM_DEF_NOTIFIED_PROP_CLASS, CTM_DEF_PROP_REF_CLASS.
#define CTM_DECL_SIMPLE_CLASS_FIELD( type, name, setterName, initializer ) \
    private: \
        type m_##name = initializer; \
    public: \
        type name() const { \
            return m_##name; \
            } \
        ThisClass& setterName( type name ) { \
            m_##name = name; \
            return *this; \
            }

#endif // _INFRA_DEF_GETSET_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

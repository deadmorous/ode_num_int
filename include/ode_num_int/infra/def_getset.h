// def_getset.h

#ifndef _INFRA_DEF_GETSET_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_DEF_GETSET_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#define CTM_DECL_THIS_CLASS_ALIAS( ClassName ) using ThisClass = ClassName;

// Use CTM_DECL_THIS_CLASS_ALIAS before using the macro below
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

// def_prop_vd_template_class.h

/// \file
/// \brief Macros to declare property holder classes with one template parameter.

#ifndef _INFRA_DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../la/VectorTemplate.h"
#include "../infra/def_prop_class.h"

/// \brief Declares a template class with one template parameter, a single private field and public getter and setter methods for it.
/// \param ClassName The name of the template class being declared.
/// \param StoredClassName The name of the class to be held in the private field.
/// \param getter The name of the getter method.
/// \param setter The name of the setter method.
/// \note The private field is of type std::shared_ptr< StoredClassName<VD> >, where VD is the template
/// parameter of the class being declared.
/// \note This macro uses the CTM_DEF_PROP_CLASS macro.
/// \sa CTM_DEF_PROP_CLASS, CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS.
#define CTM_DEF_PROP_VD_TEMPLATE_CLASS(ClassName, StoredClassName, getter, setter) \
    template< class VD > CTM_DEF_PROP_CLASS( \
        ClassName, \
        std::shared_ptr< StoredClassName<VD> >, std::shared_ptr< StoredClassName<VD> >, \
        getter, setter )

/// \brief Declares a template class with one template parameter, a single private field and public getter, setter, and observer for value modification.
/// \param ClassName The name of the template class being declared.
/// \param StoredClassName The name of the class to be held in the private field.
/// \param getter The name of the getter method.
/// \param setter The name of the setter method.
/// \param addOnChangeObserver The name of the method to add modification observer.
/// \param removeOnChangeObserver The name of the method to remove modification observer.
/// \note The private field is of type std::shared_ptr< StoredClassName<VD> >, where VD is the template
/// parameter of the class being declared.
/// \note This macro uses the CTM_DEF_NOTIFIED_PROP_CLASS macro.
/// \sa CTM_DEF_PROP_CLASS, CTM_DEF_PROP_VD_TEMPLATE_CLASS.
#define CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(ClassName, StoredClassName, getter, setter, addOnChangeObserver, removeOnChangeObserver) \
    template< class VD > CTM_DEF_NOTIFIED_PROP_CLASS( \
        ClassName, \
        std::shared_ptr< StoredClassName<VD> >, std::shared_ptr< StoredClassName<VD> >, \
        getter, setter, addOnChangeObserver, removeOnChangeObserver )

#endif // _INFRA_DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

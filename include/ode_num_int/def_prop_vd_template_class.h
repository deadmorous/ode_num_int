// def_prop_vd_template_class.h

#ifndef _DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "vector_util.h"
#include "ctmstd/def_prop_class.h"

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

#endif // _DEF_PROP_VD_TEMPLATE_CLASS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

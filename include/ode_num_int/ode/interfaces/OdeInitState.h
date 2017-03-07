// OdeInitState.h

#ifndef _ODE_INTERFACES_ODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/def_prop_vd_template_class.h"
#include "../../infra/Factory.h"
#include "../../infra/OptionalParameters.h"

namespace ctm {
namespace math {

template< class VD >
class OdeInitState :
    public Factory< OdeInitState<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        virtual real_type initialTime() const = 0;
        virtual V initialState() const = 0;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

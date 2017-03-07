// NewtonSolverIterationObservers.h

#ifndef _ALG_INTERFACES_NEWTONSOLVERITERATIONOBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_NEWTONSOLVERITERATIONOBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/cxx_observers.h"

namespace ctm {
namespace math {

template< class VD > class NewtonSolverInterface;

template< class VD >
using NewtonSolverIterationObservers = cxx::TerminatingObservers<
        unsigned int /*iterationNumber*/,
        const NewtonSolverInterface<VD>* /*solver*/>;

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_NEWTONSOLVERITERATIONOBSERVERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

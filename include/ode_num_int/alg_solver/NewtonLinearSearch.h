// NewtonLinearSearch.h

#ifndef _ALGSOLVER_NEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_NEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "VectorMapping.h"

namespace ctm {
namespace math {

template< class VD >
using SimpleLineSearchObservers = cxx::Observers<
        unsigned int /*iteration*/,
        typename VD::value_type /*alpha*/,
        const VectorTemplate< VD >& /*dir*/,
        const VectorTemplate< VD >& /*x0*/,
        const VectorTemplate< VD >& /*f0*/,
        const VectorTemplate< VD >& /*f*/ >;



template< class VD >
class NewtonLinearSearch :
    public MappingHolder< VD >,
    public Factory< NewtonLinearSearch<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        SimpleLineSearchObservers< VD > lineSearchObservers;

        virtual real_type lineSearch( const V& dir, const V& x0, const V& f0, V *residual = nullptr ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonLinearSearchHolder, NewtonLinearSearch,
        newtonLinearSearch, setNewtonLinearSearch,
        onNewtonLinearSearchChanged, offNewtonLinearSearchChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_NEWTONLINEARSEARCH_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

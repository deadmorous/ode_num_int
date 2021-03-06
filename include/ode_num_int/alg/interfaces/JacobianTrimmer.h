// JacobianTrimmer.h

#ifndef _ALG_INTERFACES_JACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_JACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./VectorMapping.h"
#include "../../la/SparseMatrixTemplate.h"

namespace ctm {
namespace math {

template< class VD >
class JacobianTrimmer :
    public Factory< JacobianTrimmer<VD> >,
    public MappingHolder< VD >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrix< real_type > SparseMatrix;

        virtual SparseMatrix trimJacobian( const SparseMatrix& J ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        JacobianTrimmerHolder, JacobianTrimmer,
        jacobianTrimmer, setJacobianTrimmer,
        onJacobianTrimmerChanged, offJacobianTrimmerChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_JACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

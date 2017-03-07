// DefaultJacobianProvider.h

#ifndef _ALG_DEFAULTJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_DEFAULTJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/JacobianProvider.h"
#include "./computeJacobian.h"

namespace ctm {
namespace math {

template< class VD >
class DefaultJacobianProvider :
    public JacobianProvider<VD>,
    public FactoryMixin< DefaultJacobianProvider<VD>, JacobianProvider<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > FastSparseMatrix;

        void computeJacobian( const V& x0 )
            {
            if( auto jt = this->jacobianTrimmer() )
                m_J = jt->trimJacobian( math::computeJacobian( *this->mapping(), x0 ) );
            else
                m_J = math::computeJacobian( *this->mapping(), x0 );
            }

        FastSparseMatrix& jacobian() {
            return m_J;
            }

    private:
        FastSparseMatrix m_J;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALG_DEFAULTJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// SparseJacobianProvider.h

#ifndef _ALGSOLVER_SPARSEJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_SPARSEJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/JacobianProvider.h"
#include "computeJacobian.h"
#include "SparseJacobianCalculator.h"

namespace ctm {
namespace math {

template< class VD >
class SparseJacobianProvider :
    public JacobianProvider<VD>,
    public FactoryMixin< SparseJacobianProvider<VD>, JacobianProvider<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > FastSparseMatrix;

        SparseJacobianProvider() :
            m_Jcalc( &m_J )
            {}

        SparseJacobianProvider( const SparseJacobianProvider<VD>& ) = delete;
        SparseJacobianProvider<VD>& operator=( const SparseJacobianProvider<VD>& ) = delete;

        void hardReset() {
            m_J = FastSparseMatrix();
        }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeJacobian( const V& x0 )
            {
            if( auto jt = this->jacobianTrimmer() )
                m_J = jt->trimJacobian( math::computeJacobian( *this->mapping(), x0 ) );
            else if( m_J.empty() ) {
                m_J = math::computeJacobian( *this->mapping(), x0 );
                m_Jcalc = SparseJacobianCalculator<VD>( &m_J );
                }
            else
                m_Jcalc.calculate( *this->mapping(), x0, 1e-6 );
            }

        FastSparseMatrix& jacobian() {
            return m_J;
            }

    private:
        FastSparseMatrix m_J;
        SparseJacobianCalculator< VD > m_Jcalc;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_SPARSEJACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// JacobianProvider.h

#ifndef _ALG_INTERFACES_JACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_INTERFACES_JACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./JacobianTrimmer.h"

namespace ctm {
namespace math {

template< class VD >
class JacobianProvider :
    public MappingHolder< VD >,
    public JacobianTrimmerHolder< VD >,
    public Factory< JacobianProvider<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > FastSparseMatrix;

        JacobianProvider()
            {
            auto updateTrimmerMapping = [this]() {
                if( auto jt = this->jacobianTrimmer() )
                    jt->setMapping( this->mapping() );
                };
            this->onMappingChanged( updateTrimmerMapping );
            this->onJacobianTrimmerChanged( updateTrimmerMapping );
            }

        virtual void hardReset() {}
        virtual bool hardResetMayHelp() const {
            return false;
            }
        virtual void computeJacobian( const V& x0 ) = 0;
        virtual FastSparseMatrix& jacobian() = 0;
        const FastSparseMatrix& jacobian() const {
            return const_cast< JacobianProvider<VD>* >( this )->jacobian();
            }

    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        JacobianProviderHolder, JacobianProvider,
        jacobianProvider, setJacobianProvider,
        onJacobianProviderChanged, offJacobianProviderChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALG_INTERFACES_JACOBIANPROVIDER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// NewtonDescentDirection.h

#ifndef _ALGSOLVER_NEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_NEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "JacobianProvider.h"
#include "../../lu/LUFactorizerTimingStats.h"
#include "../../la/sparse_util.h"

namespace ctm {
namespace math {

using DescentDirectionPreObservers = cxx::Observers<>;

template< class VD >
using DescentDirectionPostObservers = cxx::Observers< const VectorTemplate< VD >& /*dir*/ >;

template< class VD >
using JacobianObservers = cxx::Observers<
    const sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData< typename VD::value_type > >& /*J*/,
    const VectorTemplate< VD >& /*x0*/, const VectorTemplate< VD >& /*f0*/ >;

using JacobianRefreshObservers = cxx::Observers< bool /*start*/ >;



template< class VD >
class NewtonDescentDirection :
    public MappingHolder< VD >,
    public JacobianProviderHolder< VD >,
    public Factory< NewtonDescentDirection<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        DescentDirectionPreObservers ddirPreObservers;
        DescentDirectionPostObservers< VD > ddirPostObservers;
        JacobianObservers<VD> jacobianObservers;
        JacobianRefreshObservers jacobianRefreshObservers;
        LUFactorizerTimingStats luTimingStats;

        NewtonDescentDirection() {
            auto setJpMapping = [this] {
                auto jp = this->jacobianProvider();
                if( jp )
                    jp->setMapping( this->mapping() );
                };
            this->onMappingChanged( setJpMapping );
            this->onJacobianProviderChanged( setJpMapping );
            }

        virtual void reset( bool hard ) = 0;
        virtual bool hardResetMayHelp() const = 0;
        virtual void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int iterationNumber ) = 0;

        // Called to inform this instance about success or failure of an attempt to solve the equation.
        virtual void reportSuccess( bool success ) {}

        V computeDescentDirection( const V& x0, const V& f0, unsigned int iterationNumber )
            {
            V result;
            computeDescentDirection( result, x0, f0, iterationNumber );
            return result;
            }
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonDescentDirectionHolder, NewtonDescentDirection,
        newtonDescentDirection, setNewtonDescentDirection,
        onNewtonDescentDirectionChanged, offNewtonDescentDirectionChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_NEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// OdeRhs.h

#ifndef _ODE_INTERFACES_ODERHS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODERHS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/def_prop_vd_template_class.h"
#include "../../infra/Factory.h"
#include "../../infra/OptionalParameters.h"

namespace ctm {
namespace math {

template< class VD > class OdeRhs;

template< class VD >
using OdeRhsPreObservers = cxx::Observers<
    typename VD::value_type /*time*/,
    const VectorTemplate< VD >& /*argument*/,
    const OdeRhs<VD>* /*odeRhs*/ >;

template< class VD >
using OdeRhsPostObservers = cxx::Observers<
    typename VD::value_type /*time*/,
    const VectorTemplate< VD >& /*argument*/,
    const VectorTemplate< VD >& /*value*/,
    const OdeRhs<VD>* /*odeRhs*/ >;

template< class VD >
class OdeRhs :
    public Factory< OdeRhs<VD> >,
    public OptionalParameters
    {
    public:
        enum ZeroFunctionFlag {
            PlusMinus = 0x01,
            MinusPlus = 0x02,
            BothDirections = PlusMinus | MinusPlus,
            RecomputeAfterSwitch = 0x04,
            InterpolateAfterSwitch = 0x00
        };

        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        OdeRhsPreObservers<VD> odeRhsPreObservers;
        OdeRhsPostObservers<VD> odeRhsPostObservers;

        V rhs( real_type time, const V& x ) const {
            V result( varCount() );
            rhs( result, time, x );
            return result;
            }
        V operator()( real_type time, const V& x ) const {
            return rhs( time, x );
            }

        virtual unsigned int secondOrderVarCount() const = 0;
        virtual unsigned int firstOrderVarCount() const = 0;
        virtual unsigned int zeroFuncCount() const {
            return 0;
            }
        virtual std::vector<unsigned int> zeroFuncFlags() const {
            return std::vector<unsigned int>( zeroFuncCount(), BothDirections );
        }

        virtual void rhs( V& dst, real_type time, const V& x ) const = 0;
        virtual void zeroFunctions( V& /*dst*/, real_type /*time*/, const V& /*x*/ ) const {}
        virtual void switchPhaseState( const int* /*transitions*/, real_type /*time*/, V& /*x*/ ) {}
        virtual std::string describeZeroFunction( unsigned int /*index*/ ) const {
            return std::string();
            }

        virtual std::vector<unsigned int> varIds() const {
            return std::vector<unsigned int>( varCount(), 0 );
            }
        virtual void beforeStep( real_type /*time*/, const V& /*x*/ ) const {}
        virtual void beforeStep2( const V& /*rhs*/ ) const {}
        virtual bool truncateStep( real_type& /*tEnd*/, V& /*xEnd*/ ) const {
            return false;
        }

        unsigned int varCount() const {
            return 2*secondOrderVarCount() + firstOrderVarCount();
            }
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(OdeRhsHolder, OdeRhs, odeRhs, setOdeRhs, onOdeRhsChanged, offOdeRhsChanged)

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODERHS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

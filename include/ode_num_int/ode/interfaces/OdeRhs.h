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
class OdeRhs : // OdeRhs интерфейс правосторонней ОДУ
    public Factory< OdeRhs<VD> >,
    public OptionalParameters
    {
    public:
        enum ZeroFunctionFlag { // нулевая функция, которая почти везде равна 0; enum - перечисения
            PlusMinus = 0x01, // в шестнадцатиричной системе счисления
            MinusPlus = 0x02,
            BothDirections = PlusMinus | MinusPlus,
            RecomputeAfterSwitch = 0x04,
            InterpolateAfterSwitch = 0x00
        };

        typedef VectorTemplate< VD > V; // typedef - присваивание V типа данных VectorTemplate< VD >
        typedef typename V::value_type real_type; // typename указывает на то что value_type - тип данных, тогда real_type - новое имя для этого типа данных
        OdeRhsPreObservers<VD> odeRhsPreObservers;
        OdeRhsPostObservers<VD> odeRhsPostObservers;

        V rhs( real_type time, const V& x ) const { // & - адресс, берется для того чтобы функция, использующая переменную в качестве параметра, могла что-то в нее записать
            V result( varCount() ); // функц result типа данных V зависящая от числа производных
            rhs( result, time, x );
            return result;
            }
        V operator()( real_type time, const V& x ) const {
            return rhs( time, x );
            }

        virtual unsigned int secondOrderVarCount() const = 0; // virtual - виртуальная функция базового класса, которая переопределяеся производным классом
        virtual unsigned int firstOrderVarCount() const = 0; // unsigned - указывает что переменная может принимать только неотрицательные значения
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

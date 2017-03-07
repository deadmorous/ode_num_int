// VectorMapping.h

#ifndef _ALGSOLVER_VECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_VECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/def_prop_vd_template_class.h"
#include "../../infra/Factory.h"
#include "../../infra/OptionalParameters.h"

namespace ctm {
namespace math {

template< class VD > class VectorMapping;

template< class VD >
using VectorMappingPreObservers = cxx::Observers< const VectorTemplate< VD >& /*argument*/, const VectorMapping<VD>* /*mapping*/ >;

template< class VD >
using VectorMappingPostObservers = cxx::Observers< const VectorTemplate< VD >& /*argument*/, const VectorTemplate< VD >& /*value*/, const VectorMapping<VD>* /*mapping*/ >;

template< class VD >
class VectorMapping :
    public OptionalParameters
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;
        VectorMappingPreObservers<VD> vectorMappingPreObservers;
        VectorMappingPostObservers<VD> vectorMappingPostObservers;

        virtual ~VectorMapping() {}

        virtual unsigned int inputSize() const = 0;
        virtual unsigned int outputSize() const = 0;
        virtual void map( V& dst, const V& x ) const = 0;
        virtual std::vector<unsigned int> inputIds() const {
            return std::vector<unsigned int>( inputSize(), 0 );
            }
        virtual std::vector<unsigned int> outputIds() const {
            return std::vector<unsigned int>( outputSize(), 0 );
            }

        V map( const V& x ) const {
            V result( outputSize() );
            map( result, x );
            return result;
            }
        V operator()( const V& x ) const {
            return map( x );
            }
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(MappingHolder, VectorMapping, mapping, setMapping, onMappingChanged, offMappingChanged)

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_VECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

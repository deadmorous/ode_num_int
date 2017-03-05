// VectorMappingRegularizer.h

#ifndef _ALGSOLVER_VECTORMAPPINGREGULARIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_VECTORMAPPINGREGULARIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "opt_param.h"
#include "ctmstd/def_prop_class.h"

namespace ctm {
namespace math {

class VectorMappingRegularizer :
    public Factory< VectorMappingRegularizer >,
    public OptionalParameters
    {
    public:
        virtual bool isRegularizationAvailable() const = 0;
        virtual void applyRegularizationParameter( double param ) = 0;
    };
CTM_DEF_PROP_CLASS(
        VectorMappingRegularizerHolder,
        std::shared_ptr< VectorMappingRegularizer >, std::shared_ptr< VectorMappingRegularizer >,
        regularizer, setRegularizer )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_VECTORMAPPINGREGULARIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

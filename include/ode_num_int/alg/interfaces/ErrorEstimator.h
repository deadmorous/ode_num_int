// ErrorEstimator.h

#ifndef _ALGSOLVER_ERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_ERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../infra/def_prop_vd_template_class.h"
#include "../../infra/OptionalParameters.h"

namespace ctm {
namespace math {

template< class VD > class ErrorEstimator;

template< class VD >
using ErrorObservers = cxx::Observers< const VectorTemplate< VD >& /*solution*/, const VectorTemplate< VD >& /*residual*/, const ErrorEstimator<VD>* >;

template< class VD >
class ErrorEstimator :
    public Factory< ErrorEstimator<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        ErrorObservers<VD> errorObservers;

        enum Status {
            Converged,
            ContinueIterations,
            Diverged
        };

        virtual void reset( const V& initialGuess ) = 0;
        virtual void setCurrentSolution( const V& solution, const V& residual ) = 0;
        virtual Status currentStatus() const = 0;
        virtual real_type absoluteError() const = 0;
        virtual real_type relativeError() const = 0;
        virtual real_type absoluteTolerance() const = 0;
        virtual void setAbsoluteTolerance( real_type absoluteTolerance ) = 0;
        virtual real_type relativeTolerance() const = 0;
        virtual void setRelativeTolerance( real_type relativeTolerance ) = 0;
        virtual std::string currentStatusMessage() const = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        ErrorEstimatorHolder, ErrorEstimator,
        errorEstimator, setErrorEstimator,
        onErrorEstimatorChanged, offErrorEstimatorChanged )

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        ConstErrorEstimatorHolder, const ErrorEstimator,
        errorEstimator, setErrorEstimator,
        onErrorEstimatorChanged, offErrorEstimatorChanged )

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_ERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// OdeEventController.h

#ifndef _ODESOLVER_ODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeRhs.h"

namespace ctm {
namespace math {

template< class VD >
class OdeEventController :
    public OdeRhsHolder<VD>,
    public Factory< OdeEventController<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef std::function<void(V&, real_type)> Interpolator;

        virtual void reset() = 0;
        virtual void atStepStart( const V& x1, real_type t1 ) = 0;
        virtual bool atStepEnd(
                const V& x1, V& x2, real_type& t2, Interpolator interpolate,
                unsigned int *izfTrunc = nullptr, int *transitionType = nullptr ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        OdeEventControllerHolder, OdeEventController,
        eventController, setEventController,
        onEventControllerChanged, offEventControllerChanged)

template< class VD >
inline typename OdeEventController<VD>::Interpolator linearInterpolator(
        const VectorTemplate< VD >& x1, const VectorTemplate< VD >& x2 )
    {
    return [&]( VectorTemplate< VD >& dst, typename VD::value_type t ) {
        // have to use const_cast because of lack of design
        typedef VectorTemplate< VD > V;
        dst = const_cast<V&>(x1).scaled( 1-t );
        dst += const_cast<V&>(x2).scaled( t );
        };
    }

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODEEVENTCONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

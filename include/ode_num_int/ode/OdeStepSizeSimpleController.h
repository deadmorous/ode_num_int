// OdeStepSizeSimpleController.h

#ifndef _ODESOLVER_ODESTEPSIZESIMPLECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESTEPSIZESIMPLECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeStepSizeController.h"
#include "ctmstd/m_tolerance.h"

namespace ctm {
namespace math {

template< class VD >
class OdeStepSizeSimpleController :
    public OdeStepSizeController<VD>,
    public FactoryMixin< OdeStepSizeSimpleController<VD>, OdeStepSizeController<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef typename OdeStepSizeSimpleController<VD>::Result Result;
        typedef OptionalParameters::Parameters Parameters;

        OdeStepSizeSimpleController() :
            m_tolerance( 1e-6 ),
            m_insuranceCoefficient( 0.8 ),
            m_incMinFactor( 1.3 ),
            m_incMaxFactor( 2 ),
            m_decMaxFactor( 0.1 ),
            m_maxThreshold( 0 )
            {}

        Result controlStepSize(
            real_type currentStepSize,
            real_type errorNorm,
            real_type methodOrder ) const
            {
            if( errorNorm == 0 )
                return Result( currentStepSize, true, false );
            real_type p = std::pow( m_tolerance/errorNorm, 1./(methodOrder+1) );
            real_type pd = p * m_insuranceCoefficient;
            if( p >= 1 ) {
                if( m_maxThreshold > 0   &&   pd*currentStepSize > m_maxThreshold ) {
                    pd = m_maxThreshold / currentStepSize;
                    if( pd < 1 )
                        return Result( currentStepSize, true, false );
                    }
                if( m_incMinFactor > 0   &&   pd < m_incMinFactor )
                    return Result( currentStepSize, true, false );
                if( m_incMaxFactor > 0   &&   pd > m_incMaxFactor )
                    pd = m_incMaxFactor;
                return Result( currentStepSize*pd, true, true );
                }
            else if( p > 1 - GEOM_EPS )
                return Result( currentStepSize, true, false );
            else {
                if( m_decMaxFactor > 0   &&   pd < m_decMaxFactor )
                    pd = m_decMaxFactor;
                return Result( currentStepSize*pd, false, true );
                }
            }

        Parameters parameters() const
            {
            Parameters result;
            result["tolerance"] = m_tolerance;
            result["insurance"] = m_insuranceCoefficient;
            result["inc_min_factor"] = m_incMinFactor;
            result["inc_max_factor"] = m_incMaxFactor;
            result["dec_max_factor"] = m_decMaxFactor;
            result["max_threshold"] = m_maxThreshold;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "tolerance", m_tolerance );
            this->maybeLoadParameter( parameters, "insurance", m_insuranceCoefficient );
            this->maybeLoadParameter( parameters, "inc_min_factor", m_incMinFactor );
            this->maybeLoadParameter( parameters, "inc_max_factor", m_incMaxFactor );
            this->maybeLoadParameter( parameters, "dec_max_factor", m_decMaxFactor );
            this->maybeLoadParameter( parameters, "max_threshold", m_maxThreshold );
            }

    private:
        real_type m_tolerance;
        real_type m_insuranceCoefficient;
        real_type m_incMinFactor;
        real_type m_incMaxFactor;
        real_type m_decMaxFactor;
        real_type m_maxThreshold;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESTEPSIZESIMPLECONTROLLER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

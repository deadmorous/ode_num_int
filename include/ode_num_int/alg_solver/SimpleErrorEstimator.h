// SimpleErrorEstimator.h

#ifndef _ALGSOLVER_SIMPLEERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_SIMPLEERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "ErrorEstimator.h"

#include <sstream>

namespace ctm {
namespace math {

template< class VD >
class SimpleErrorEstimator :
    public ErrorEstimator< VD >,
    public FactoryMixin< SimpleErrorEstimator<VD>, ErrorEstimator<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef typename ErrorEstimator< VD >::Status Status;

        SimpleErrorEstimator() :
            m_absoluteTolerance( 1e-6 ),
            m_relativeTolerance( 1e-6 ),
            m_absoluteMaxThreshold( 1e20 ),
            m_relativeMaxThreshold( 10 ),
            m_absError( 0 ),
            m_relError( 0 )
            {}

        void reset( const V& initialGuess )
            {
            m_currentSolution = initialGuess;
            m_absError = 0;
            m_relError = 0;
            }

        void setCurrentSolution( const V& solution, const V& residual )
            {
            V dx = solution - m_currentSolution;
            real_type xnorm = m_currentSolution.infNorm();
            m_relError = xnorm > 0 ?   dx.infNorm() / xnorm :   0;
            m_currentSolution = solution;
            m_absError = residual.infNorm();
            this->errorObservers( solution, residual, this );
            }

        Status currentStatus() const
            {
            if( m_absError <= m_absoluteTolerance   &&   m_relError <= m_relativeTolerance )
                return ErrorEstimator<VD>::Converged;
            if( m_absoluteMaxThreshold > 0   &&   m_absError >= m_absoluteMaxThreshold )
                return ErrorEstimator<VD>::Diverged;
            if( m_relativeMaxThreshold > 0   &&   m_relError >= m_relativeMaxThreshold )
                return ErrorEstimator<VD>::Diverged;
            return ErrorEstimator<VD>::ContinueIterations;
            }

        real_type absoluteError() const {
            return m_absError;
            }

        real_type relativeError() const {
            return m_relError;
            }

        real_type absoluteTolerance() const {
            return m_absoluteTolerance;
            }

        void setAbsoluteTolerance( real_type absoluteTolerance ) {
            m_absoluteTolerance = absoluteTolerance;
            }

        real_type relativeTolerance() const {
            return m_relativeTolerance;
            }

        void setRelativeTolerance( real_type relativeTolerance ) {
            m_relativeTolerance = relativeTolerance;
            }

        std::string currentStatusMessage() const
            {
            auto statusText = []( Status status ) -> std::string {
                switch ( status ) {
                    case SimpleErrorEstimator::Converged: return "converged";
                    case SimpleErrorEstimator::ContinueIterations: return "continue";
                    case SimpleErrorEstimator::Diverged:   return "diverged";
                    default:
                        ASSERT( false );
                        return "unknown";
                    }
                };

            std::ostringstream s;
            s << "Satus: " << statusText(currentStatus()) << ", abs. error: " << m_absError << ", rel. error: " << m_relError;
            return s.str();
            }

        real_type absoluteMaxThreshold() const {
            return m_absoluteMaxThreshold;
            }

        void setAbsoluteMaxThreshold( real_type absoluteMaxThreshold ) {
            m_absoluteMaxThreshold = absoluteMaxThreshold;
            }

        real_type relativeMaxThreshold() const {
            return m_relativeMaxThreshold;
            }

        void setRelativeMaxThreshold( real_type relativeMaxThreshold ) {
            m_relativeMaxThreshold = relativeMaxThreshold;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["abs_tol"] = m_absoluteTolerance;
            result["rel_tol"] = m_relativeTolerance;
            result["abs_max_threshold"] = m_absoluteMaxThreshold;
            result["rel_max_threshold"] = m_relativeMaxThreshold;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters & parameters )
            {
            OptionalParameters::maybeLoadParameter( parameters, "abs_tol", m_absoluteTolerance );
            OptionalParameters::maybeLoadParameter( parameters, "rel_tol", m_relativeTolerance );
            OptionalParameters::maybeLoadParameter( parameters, "abs_max_threshold", m_absoluteMaxThreshold );
            OptionalParameters::maybeLoadParameter( parameters, "rel_max_threshold", m_relativeMaxThreshold );
            }

    private:
        real_type m_absoluteTolerance;
        real_type m_relativeTolerance;
        real_type m_absoluteMaxThreshold;
        real_type m_relativeMaxThreshold;

        V m_currentSolution;
        real_type m_absError;
        real_type m_relError;
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_SIMPLEERRORESTIMATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

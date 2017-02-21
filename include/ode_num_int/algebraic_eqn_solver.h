// algebraic_eqn_solver.h

#ifndef _ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "vector_mapping.h"
#include "lu_util.h"
#include "sparse_jacobian.h"

#include <sstream>

namespace ctm {
namespace math {

template< class Mapping, class V >
inline sparse::SparseMatrix< typename V::value_type > computeJacobian( const Mapping& mapping, const V& x0 )
    {
    typedef typename V::value_type real_type;

    // Determine problem size
    auto n = mapping.inputSize();
    ASSERT( x0.size() == n );
    ASSERT( mapping.outputSize() == n );

    // Allocate storage
    V x(n), f0(n), f(n);

    // Compute rhs at x0
    mapping.map( f0, x0 );

    auto makeX = [&]( unsigned int i, real_type dx ) {
        x = x0;
        x[i] += dx;
        };

    // Compute the Jacobian
    real_type dx = 1e-6;
    sparse::SparseMatrix< typename V::value_type > result( n, n );
    for( unsigned int i=0; i<n; ++i ) {
        makeX( i, dx );
        mapping.map( f, x );
        for( unsigned int j=0; j<n; ++j ) {
            auto df = f[j] - f0[j];
            if( df == 0 )
                continue;
            result.at( j, i ) = df / dx;
            }
        }

    return result;
    }

template< class VD >
class JacobianTrimmer :
    public Factory< JacobianTrimmer<VD> >,
    public MappingHolder< VD >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrix< real_type > SparseMatrix;

        virtual SparseMatrix trimJacobian( const SparseMatrix& J ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        JacobianTrimmerHolder, JacobianTrimmer,
        jacobianTrimmer, setJacobianTrimmer,
        onJacobianTrimmerChanged, offJacobianTrimmerChanged )



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



template< class VD >
class DefaultJacobianProvider :
    public JacobianProvider<VD>,
    public FactoryMixin< DefaultJacobianProvider<VD>, JacobianProvider<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > FastSparseMatrix;

        void computeJacobian( const V& x0 )
            {
            if( auto jt = this->jacobianTrimmer() )
                m_J = jt->trimJacobian( math::computeJacobian( *this->mapping(), x0 ) );
            else
                m_J = math::computeJacobian( *this->mapping(), x0 );
            }

        FastSparseMatrix& jacobian() {
            return m_J;
            }

    private:
        FastSparseMatrix m_J;
    };

template< class VD >
class SparseJacobianProvider :
    public JacobianProvider<VD>,
    public FactoryMixin< SparseJacobianProvider<VD>, JacobianProvider<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > FastSparseMatrix;

        SparseJacobianProvider() :
            m_Jcalc( &m_J )
            {}

        SparseJacobianProvider( const SparseJacobianProvider<VD>& ) = delete;
        SparseJacobianProvider<VD>& operator=( const SparseJacobianProvider<VD>& ) = delete;

        void hardReset() {
            m_J = FastSparseMatrix();
        }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeJacobian( const V& x0 )
            {
            if( auto jt = this->jacobianTrimmer() )
                m_J = jt->trimJacobian( math::computeJacobian( *this->mapping(), x0 ) );
            else if( m_J.empty() ) {
                m_J = math::computeJacobian( *this->mapping(), x0 );
                m_Jcalc = SparseJacobianCalculator<VD>( &m_J );
                }
            else
                m_Jcalc.calculate( *this->mapping(), x0, 1e-6 );
            }

        FastSparseMatrix& jacobian() {
            return m_J;
            }

    private:
        FastSparseMatrix m_J;
        SparseJacobianCalculator< VD > m_Jcalc;
    };

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



template< class VD >
class JacobianWidthBasedTrimmer :
    public JacobianTrimmer<VD>,
    public FactoryMixin< JacobianWidthBasedTrimmer<VD>, JacobianTrimmer<VD> >
    {
    public:
        typedef typename JacobianTrimmer<VD>::SparseMatrix SparseMatrix;

        JacobianWidthBasedTrimmer() : m_width(0) {}

        SparseMatrix trimJacobian( const SparseMatrix& J )
            {
            if( m_width == 0 )
                return J;
            SparseMatrix result( J.size() );
            for( const auto& e : J ) {
                int d = e.first.first - e.first.second;
                if( std::abs( d ) < static_cast<int>(m_width) )
                    result.at( e.first ) = e.second;
                }
            return result;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["width"] = m_width;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters& parameters  ) {
            OptionalParameters::maybeLoadParameter( parameters, "width", m_width );
            }

    private:
        unsigned int m_width;
    };



template< class VD >
class JacobianThresholdBasedTrimmer :
    public JacobianTrimmer<VD>,
    public FactoryMixin< JacobianThresholdBasedTrimmer<VD>, JacobianTrimmer<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef typename JacobianTrimmer<VD>::SparseMatrix SparseMatrix;

        JacobianThresholdBasedTrimmer() : m_threshold(0) {}

        SparseMatrix trimJacobian( const SparseMatrix& J )
            {
            if( m_threshold == 0 )
                return J;
            SparseMatrix result( J.size() );
            real_type jmax = 0;
            for( const auto& e : J )
                jmax = std::max( jmax, std::fabs( e.second ) );
            if( jmax == 0 )
                return J;
            auto absThreshold = m_threshold * jmax;
            for( const auto& e : J ) {
                if( e.first.first == e.first.second   ||
                    std::fabs( e.second ) >= absThreshold )
                    result.at( e.first ) = e.second;
                }
            return result;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["threshold"] = m_threshold;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters& parameters  ) {
            OptionalParameters::maybeLoadParameter( parameters, "threshold", m_threshold );
            }

    private:
        double m_threshold;
    };



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
        typename LUFactorizer<real_type>::TimingStats luTimingStats;

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



// The Jacobian is recomputed each time; linear system is solved to find the direction
template< class VD >
class SimpleNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< SimpleNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        void reset( bool hard ) {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            }

        bool hardResetMayHelp() const {
            return this->jacobianProvider()->hardResetMayHelp();
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            this->jacobianRefreshObservers( true );
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            jp->computeJacobian( x0 );
            auto& J = jp->jacobian();
            this->jacobianRefreshObservers( false );
            this->jacobianObservers( J, x0, f0 );
            this->ddirPreObservers();
            if( m_lu.empty() )
                m_lu.setMatrix( J );
            else
                m_lu.setMatrixFast( J );
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
    };

// The Jacobian is computed for the first time;
// then it is updated each next time using the "Broyden good" formula;
// linear system is solved to find the direction.
// The main downside is that the Jacobian becomes dense - do not use in production code!
template< class VD >
class JacobianBroydenUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianBroydenUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianBroydenUpdateNewtonDescentDirection() :
            m_havePrev( false )
            {}

        void reset( bool hard )
            {
            if( hard )
                this->jacobianProvider()->hardReset();
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            auto& J = jp->jacobian();
            auto recalcJacobian = J.empty();
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                jp->computeJacobian( x0 );
                this->jacobianRefreshObservers( false );
                }
            this->ddirPreObservers();
            if( !recalcJacobian   &&   m_havePrev ) {
                V dx = x0 - m_xprev;
                V df = f0 - m_fprev;
                V left = df - J*dx;
                left *= 1 / dx.euclideanNormSquare();
                unsigned int n = x0.size();
                ASSERT( J.size() == sparse::SparseMatrixCommonTypes::Index(n, n) );
                for( unsigned int i=0; i<n; ++i )
                    for( unsigned int j=0; j<n; ++j ) {
                        real_type x = left[i] * dx[j];
                        if( x != 0 )
                            J.at( i, j ) += x;
                        }
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            this->jacobianObservers( J, x0, f0 );
            LUFactorizer<real_type> lu( J );
            dir = f0;
            lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += lu.timingStats();
            }

    private:
        V m_xprev;
        V m_fprev;
        bool m_havePrev;
    };

// The Jacobian is computed for the first time;
// then it is updated each next time using the "Broyden good" formula, but elements
// that were zero in the initial Jacobian are forced to remain zero, so the formula is incorrect.
// However, the sparsity of the Jacobian is preserved.
// Linear system is solved to find the direction.
template< class VD >
class JacobianFakeBroydenUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianFakeBroydenUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianFakeBroydenUpdateNewtonDescentDirection() :
            m_havePrev( false )
            {}

        void reset( bool hard )
            {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            auto& J = jp->jacobian();
            auto recalcJacobian = J.empty();
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                jp->computeJacobian( x0 );
                this->jacobianRefreshObservers( false );
                }
            this->ddirPreObservers();
            if( !recalcJacobian   &&   m_havePrev ) {
                V dx = x0 - m_xprev;
                V df = f0 - m_fprev;
                V left = df - J*dx;
                left *= 1 / dx.euclideanNormSquare();
                unsigned int n = x0.size();
                ASSERT( J.size() == sparse::SparseMatrixCommonTypes::Index(n, n) );
                for( auto& element : J )
                    element.second += left[element.first.first] * dx[element.first.second];
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            this->jacobianObservers( J, x0, f0 );
            if( m_lu.empty() )
                m_lu.setMatrix( J );
            else
                m_lu.setMatrixFast( J );
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
        V m_xprev;
        V m_fprev;
        bool m_havePrev;
    };

// The Jacobian is computed like in JacobianFakeBroydenUpdateNewtonDescentDirection,
// but the same approximation is used unless the specified number of Newton iteration
// is exceeded, or at first iteration; then, the updated Jacobian is used. This is done that way in order to
// avoid LU factorization at each iteration.
template< class VD >
class JacobianLazyFakeBroydenUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianLazyFakeBroydenUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        JacobianLazyFakeBroydenUpdateNewtonDescentDirection() :
            m_lazyIterations( 10 ),
            m_havePrev( false ),
            m_lastIterationNumber( 0 ),
            m_jacobianAction ( RecomputeJacobian )
            {}

        void reset( bool hard )
            {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                m_jacobianAction = RecomputeJacobian;
                }
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int iterationNumber )
            {
            m_lastIterationNumber = iterationNumber;
            m_lu.clearTimingStats();
            auto jp = this->jacobianProvider();
            ASSERT( jp );
            auto& J = jp->jacobian();
            bool recalcJacobian = iterationNumber == 0   &&   ( J.empty()   ||   m_jacobianAction == RecomputeJacobian );
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                jp->computeJacobian( x0 );
                m_dJ = J;
                m_dJ.makeZero();
                if( m_lu.empty() )
                    m_lu.setMatrix( J );
                else
                    m_lu.setMatrixFast( J );
                this->jacobianRefreshObservers( false );
            }
            this->ddirPreObservers();
            if( !recalcJacobian   &&   m_havePrev ) {
                V dx = x0 - m_xprev;
                V df = f0 - m_fprev;
                V left = df - J*dx;
                left *= 1 / dx.euclideanNormSquare();
                unsigned int n = x0.size();
                ASSERT( m_dJ.size() == sparse::SparseMatrixCommonTypes::Index(n, n) );
                for( auto& element : m_dJ )
                    element.second += left[element.first.first] * dx[element.first.second];
                }
            if( ( iterationNumber == 0   &&   m_jacobianAction == FBUpdateJacobian ) ) {
                J += m_dJ;
                m_dJ.makeZero();
                m_lu.setMatrixFast( J );
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            this->jacobianObservers( J, x0, f0 );
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

        void reportSuccess( bool success ) {
            m_jacobianAction = success ?
                        ( m_lastIterationNumber+1 <= m_lazyIterations ? LeaveJacobian : FBUpdateJacobian ) :
                        RecomputeJacobian;
            }

        Parameters parameters() const
            {
            Parameters result;
            result["lazy_iterations"] = m_lazyIterations;
            return result;
            }

        void setParameters( const Parameters& parameters ) {
            this->maybeLoadParameter( parameters, "lazy_iterations", m_lazyIterations );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["lazy_iterations"] = "Only use the updated Jacobian when newton iteration count reaches this limit;\n"
                                        "or, if 0, use the updated Jacobian each time at first iteration.";
            return result;
            }

    private:
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData< real_type > > FastSparseMatrix;
        unsigned int m_lazyIterations;
        FastSparseMatrix m_dJ;
        LUFactorizer<real_type> m_lu;
        V m_xprev;
        V m_fprev;
        bool m_havePrev;

        enum { LeaveJacobian, FBUpdateJacobian, RecomputeJacobian } m_jacobianAction;
        unsigned int m_lastIterationNumber;
    };

// The Jacobian is computed for the first time;
// then it is updated each next time using the Hart formula, see LUFactorizer::secantUpdateHart().
// Linear system is solved to find the direction.
template< class VD >
class JacobianHartUpdateNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< JacobianHartUpdateNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        JacobianHartUpdateNewtonDescentDirection() :
            m_havePrev( false )
            {}

        void reset( bool hard )
            {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            m_havePrev = false;
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            bool recalcJacobian = !m_lu.isFactorized();
            if( recalcJacobian ) {
                this->jacobianRefreshObservers( true );
                auto jp = this->jacobianProvider();
                ASSERT( jp );
                auto& J = jp->jacobian();
                jp->computeJacobian( x0 );
                m_lu.setMatrix( J );
                this->jacobianRefreshObservers( false );
                }
            this->ddirPreObservers();
            if( !recalcJacobian    &&   m_havePrev ) {
                V s = x0 - m_xprev;
                V y = f0 - m_fprev;
                m_lu.secantUpdateHart( &*s.begin(), &*y.begin() );
                // m_lu.secantUpdateProportional( &*s.begin(), &*y.begin() );

                // deBUG, TODO: Remove
                /*
                V x = y;
                m_lu.solve( &*x.begin() );
                x -= s;
                real_type relError = x.infNorm() / s.infNorm();
                if ( relError > 1e-8 ) {
                    int a = 123;
                    }
                //*/
                }
            m_xprev = x0;
            m_fprev = f0;
            m_havePrev = true;
            if( !this->jacobianObservers.empty() )
                this->jacobianObservers( luToMatrix(m_lu), x0, f0 );
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
        V m_xprev;
        V m_fprev;
        bool m_havePrev;
    };

// The Jacobian is only computed and factorized once - at the beginning.
template< class VD >
class ConstJacobianNewtonDescentDirection :
    public NewtonDescentDirection< VD >,
    public FactoryMixin< ConstJacobianNewtonDescentDirection<VD>, NewtonDescentDirection<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        void reset( bool hard ) {
            if( hard ) {
                this->jacobianProvider()->hardReset();
                m_lu = decltype(m_lu)();
                }
            }

        bool hardResetMayHelp() const {
            return true;
            }

        void computeDescentDirection( V& dir, const V& x0, const V& f0, unsigned int /*iterationNumber*/ )
            {
            m_lu.clearTimingStats();
            if( !m_lu.isFactorized() ) {
                this->jacobianRefreshObservers( true );
                auto jp = this->jacobianProvider();
                ASSERT( jp );
                auto& J = jp->jacobian();
                jp->computeJacobian( x0 );
                m_lu.setMatrix( J );
                this->jacobianRefreshObservers( false );
                }
            if( !this->jacobianObservers.empty() )
                this->jacobianObservers( luToMatrix(m_lu), x0, f0 );
            this->ddirPreObservers();
            dir = f0;
            m_lu.solve( &*dir.begin() );
            dir *= -1;
            this->ddirPostObservers( dir );
            this->luTimingStats += m_lu.timingStats();
            }

    private:
        LUFactorizer<real_type> m_lu;
    };



template< class VD >
using SimpleLineSearchObservers = cxx::Observers<
        unsigned int /*iteration*/,
        typename VD::value_type /*alpha*/,
        const VectorTemplate< VD >& /*dir*/,
        const VectorTemplate< VD >& /*x0*/,
        const VectorTemplate< VD >& /*f0*/,
        const VectorTemplate< VD >& /*f*/ >;



template< class VD >
class NewtonLinearSearch :
    public MappingHolder< VD >,
    public Factory< NewtonLinearSearch<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        SimpleLineSearchObservers< VD > lineSearchObservers;

        virtual real_type lineSearch( const V& dir, const V& x0, const V& f0, V *residual = nullptr ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonLinearSearchHolder, NewtonLinearSearch,
        newtonLinearSearch, setNewtonLinearSearch,
        onNewtonLinearSearchChanged, offNewtonLinearSearchChanged )

template< class VD >
class SimpleNewtonLinearSearch :
    public NewtonLinearSearch< VD >,
    public FactoryMixin< SimpleNewtonLinearSearch<VD>, NewtonLinearSearch<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        SimpleNewtonLinearSearch() :
            m_maxStepTruncations( DefaultMaxStepTruncations )
            {}

        real_type lineSearch( const V& dir, const V& x0, const V& f0, V *residual = nullptr )
            {
            unsigned int iteration = 0;
            auto& m = *this->mapping();
            real_type f0Norm2 = f0.euclideanNormSquare();

            real_type alpha = 1;
            real_type alphaBest = 1;
            real_type f_f0Best = 1;
            const real_type eta = 0.8;
            unsigned int itrunc;
            auto makeNextState = [&]( real_type beta ) -> V {
                V x = x0;
                V dx = dir;
                dx *= beta;
                x += dx;
                return x;
                };
            V f;
            auto rhsNormSquare = [&]( real_type beta ) -> real_type {
                f = m( makeNextState( beta ) );
                this->lineSearchObservers( iteration++, beta, dir, x0, f0, f );
                return f.euclideanNormSquare();
                };
            for( itrunc=0; itrunc < m_maxStepTruncations; ++itrunc, alpha*=0.5 ) {
                real_type fNorm2 = rhsNormSquare( alpha );
                real_type f_f0 = fNorm2 / f0Norm2;
                if( f_f0Best > f_f0 ) {
                    f_f0Best = f_f0;
                    alphaBest = alpha;
                    if( residual )
                        *residual = f;
                    }
                if( f_f0Best < eta )
                    break;
                }
            if( itrunc == m_maxStepTruncations   &&   f_f0Best == 1 ) {
                // cout << "No convergence within max. step truncations, trying the opposite direction" << endl;
                alpha = alphaBest = -1;
                for( itrunc=0; itrunc < m_maxStepTruncations; ++itrunc, alpha*=0.5 ) {
                    real_type fNorm2 = rhsNormSquare( alpha );
                    real_type f_f0 = fNorm2 / f0Norm2;
                    if( f_f0Best > f_f0 ) {
                        f_f0Best = f_f0;
                        alphaBest = alpha;
                        if( residual )
                            *residual = f;
                        }
                    if( f_f0Best < eta )
                        break;
                    }
                if( itrunc == m_maxStepTruncations   &&   f_f0Best == 1 ) {
                    // cout << "In the opposite direction, there is still no convergence within max. step truncations" << endl;
                    alphaBest = 0;
                    }
                }
            return alphaBest;
            }

        unsigned int maxStepTruncations() const {
            return m_maxStepTruncations;
            }

        void setMaxStepTruncations( unsigned int maxStepTruncations ) {
            m_maxStepTruncations = maxStepTruncations;
            }

    private:
        const unsigned int DefaultMaxStepTruncations = 10;
        unsigned int m_maxStepTruncations;
    };



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

template< class VD >
class NewtonRegularizationStrategy :
    public VectorMappingRegularizerHolder,
    public ConstErrorEstimatorHolder<VD>,
    public NewtonDescentDirectionHolder<VD>,
    public Factory< NewtonRegularizationStrategy<VD> >,
    public OptionalParameters
    {
    public:
        virtual double regularizationParameter() const = 0;
        virtual void setRegularizationParameter( double param ) = 0;
        virtual bool adjustRegularizationParameter() = 0;
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonRegularizationStrategyHolder, NewtonRegularizationStrategy,
        regularizationStrategy, setRegularizationStrategy,
        onNewtonRegularizationStrategyChanged, offNewtonRegularizationStrategyChanged )

template< class VD >
class NewtonIterationPerformer :
    public MappingHolder< VD >,
    public ErrorEstimatorHolder< VD >,
    public NewtonDescentDirectionHolder< VD >,
    public NewtonLinearSearchHolder< VD >,
    public Factory< NewtonIterationPerformer<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        virtual void setInitialGuess( const V& initialGuess, bool hardReset = true ) = 0;
        virtual void reset( bool resetDescentDirection ) = 0;
        virtual typename ErrorEstimator<VD>::Status iteration( unsigned int iterationNumber ) = 0;
        virtual const V& currentSolution() const = 0;
        virtual const V& currentResidual() const = 0;
        virtual real_type currentStepRatio() const = 0;
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonIterationPerformerHolder, NewtonIterationPerformer,
        iterationPerformer, setIterationPerformer,
        onNewtonIterationPerformerChanged, offNewtonIterationPerformerChanged )

template< class VD >
class NewtonIterationPerformerImpl :
    public NewtonIterationPerformer< VD >,
    public FactoryMixin< NewtonIterationPerformerImpl<VD>, NewtonIterationPerformer<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        NewtonIterationPerformerImpl() :
            m_f_valid( false ),
            m_currentStepRatio( 1 ),
            m_hardResetTried( false )
            {}

        void setInitialGuess( const V& initialGuess, bool hardReset = true )
            {
            m_x = initialGuess;
            m_f_valid = false;
            reset( hardReset );
            }

        void reset( bool hardResetDescentDirection )
            {
            m_currentStepRatio = 1;
            this->errorEstimator()->reset( m_x );
            this->newtonDescentDirection()->reset( hardResetDescentDirection );
            }

        typename ErrorEstimator<VD>::Status iteration( unsigned int iterationNumber )
            {
            if( iterationNumber == 0 )
                m_hardResetTried = false;
            auto f0 = currentResidual();
            auto ddir = this->newtonDescentDirection();
            auto dir = ddir->computeDescentDirection( m_x, f0, iterationNumber );
            m_currentStepRatio = this->newtonLinearSearch()->lineSearch( dir, m_x, f0, &m_f );
            m_x += dir.scaled( m_currentStepRatio );
            this->errorEstimator()->setCurrentSolution( m_x, m_f );
            auto result = this->errorEstimator()->currentStatus();
            if( m_currentStepRatio == 0   &&   result == ErrorEstimator<VD>::ContinueIterations )
                result = ErrorEstimator<VD>::Diverged;
            return result;
            }

        const V& currentSolution() const {
            return m_x;
            }

        const V& currentResidual() const
            {
            if( !m_f_valid ) {
                m_f = this->mapping()->map( m_x );
                m_f_valid = true;
                }
            return m_f;
            }

        real_type currentStepRatio() const {
            return m_currentStepRatio;
            }

    private:
        V m_x;
        mutable V m_f;
        mutable bool m_f_valid;
        real_type m_currentStepRatio;
        bool m_hardResetTried;
    };



template< class VD >
class DiagonalMapping : public VectorBijectiveMapping< VD >
    {
    public:
        typedef VectorTemplate< VD > V;

        explicit DiagonalMapping( const V& d ) : m_d(d) {}

        unsigned int inputSize() const {
            return m_d.size();
            }

        unsigned int outputSize() const {
            return m_d.size();
            }

        void map( V& dst, const V& x ) const
            {
            ASSERT( x.size() == m_d.size() );
            auto n = x.size();
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[i] * m_d[i];
            }

        void unmap( V& dst, const V& x ) const
            {
            ASSERT( x.size() == m_d.size() );
            auto n = x.size();
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[i] / m_d[i];
            }

    private:
        V m_d;
    };



CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        DiagonalMappingHolder, DiagonalMapping,
        diagonalMapping, setDiagonalMapping,
        onDiagonalMappingChanged, offDiagonalMappingChanged )



template< class VD >
class MappingNormalizer :
    public VectorMapping< VD >,
    public MappingHolder< VD >,
    public DiagonalMappingHolder< VD >
    {
    public:
        typedef VectorTemplate< VD > V;

        unsigned int inputSize() const {
            return this->mapping()->inputSize();
            }

        unsigned int outputSize() const {
            return this->mapping()->outputSize();
            }

        virtual std::vector<unsigned int> inputIds() const {
            return this->mapping()->inputIds();
            }

        virtual std::vector<unsigned int> outputIds() const {
            return this->mapping()->outputIds();
            }

        void map( V& dst, const V& x ) const
            {
            this->vectorMappingPreObservers( x, this );
            auto& dm = *this->diagonalMapping();
            V arg = dm.VectorMapping<VD>::map( x );
            V dstUnscaled = this->mapping()->map( arg );
            dm.map( dst, dstUnscaled );
            this->vectorMappingPostObservers( x, dst, this );
            }

        void normalize( const V& x0 )
            {
            auto n = inputSize();
            V d( n );
            // TODO jacobianRefreshObservers( true );
            auto J = computeJacobian( *this->mapping(), x0 );
            // TODO jacobianRefreshObservers( true );
            for( unsigned int i=0; i<n; ++i ) {
                auto di = J.get( i, i );
                if( di == 0 )
                    throw cxx::exception( "MappingNormalizer::normalize() failed: zero diagonal element in Jacobian" );
                d[i] = 1. / sqrt( fabs( di ) );
                }
            this->setDiagonalMapping( std::make_shared< DiagonalMapping<VD> >( d ) );
            }
    };



template< class VD > class NewtonSolver;

template< class VD >
using NewtonSolverIterationObservers = cxx::TerminatingObservers<
        unsigned int /*iterationNumber*/,
        const NewtonSolver<VD>* /*solver*/>;



template< class VD >
class NewtonSolverInterface :
    public NewtonIterationPerformerHolder<VD>,
    public NewtonRegularizationStrategyHolder<VD>,
    public OptionalParameters,
    public Factory< NewtonSolverInterface<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef OptionalParameters::Parameters Parameters;

        NewtonSolverIterationObservers<VD> iterationObservers;

        enum Status {
            Converged,
            Diverged,
            Terminated,
            IterationCountLimitExceeded
        };

        virtual unsigned int iterationCountLimit() const = 0;
        virtual void setIterationCountLimit( unsigned int iterationCountLimit ) = 0;
        virtual void setInitialGuess( const V& initialGuess, bool hardReset = true ) = 0;
        virtual Status run() = 0;
        virtual const V& currentSolution() const = 0;

        static std::string statusText( Status status )
            {
            // Note: intentionally not static, to avoid the non-unloadable module problem
            struct { Status status; const char *text; } d[] = {
                { Converged, "converged" },
                { Diverged, "diverged" },
                { Terminated, "terminated" },
                { IterationCountLimitExceeded, "iteration count limit exceeded" }
                };
            for( const auto& x : d )
                if( x.status == status )
                    return x.text;
            ASSERT( false );
            return "unknown";
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonIterationPerformer<VD> >& itperf )
            {
            this->setIterationPerformer( itperf );
            auto rs = this->regularizationStrategy();
            if( rs ) {
                rs->setErrorEstimator( itperf->errorEstimator() );
                rs->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< VectorMapping<VD> >& eqnRhs )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                itperf->setMapping( eqnRhs );
                itperf->newtonDescentDirection()->setMapping( eqnRhs );
                itperf->newtonLinearSearch()->setMapping( eqnRhs );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< ErrorEstimator<VD> >& errorEstimator )
            {
            auto itperf = this->iterationPerformer();
            if( itperf )
                itperf->setErrorEstimator( errorEstimator );
            auto rs = this->regularizationStrategy();
            if( rs )
                rs->setErrorEstimator( errorEstimator );
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonDescentDirection<VD> >& ddir )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                ddir->setMapping( itperf->mapping() );
                itperf->setNewtonDescentDirection( ddir );
                }
            auto rs = this->regularizationStrategy();
            if( rs )
                rs->setNewtonDescentDirection( ddir );
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< JacobianProvider<VD> >& jacobianProvider )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                if( auto ddir = itperf->newtonDescentDirection() )
                    ddir->setJacobianProvider( jacobianProvider );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonLinearSearch<VD> >& lineSearch )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                lineSearch->setMapping( itperf->mapping() );
                itperf->setNewtonLinearSearch( lineSearch );
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< NewtonRegularizationStrategy<VD> >& regularizationStrategy )
            {
            this->setRegularizationStrategy( regularizationStrategy );
            if( regularizationStrategy ) {
                auto itperf = this->iterationPerformer();
                if( itperf ) {
                    regularizationStrategy->setErrorEstimator( itperf->errorEstimator() );
                    regularizationStrategy->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                    }
                }
            return *this;
            }

        NewtonSolverInterface<VD>& setComponent( const std::shared_ptr< JacobianTrimmer<VD> >& jacobianTrimmer )
            {
            auto itperf = this->iterationPerformer();
            if( itperf ) {
                if( auto ddir = itperf->newtonDescentDirection() ) {
                    if( auto jp = ddir->jacobianProvider() )
                        jp->setJacobianTrimmer( jacobianTrimmer );
                    }
                }
            return *this;
            }

        Parameters parameters() const
            {
            Parameters result;
            auto itperf = this->iterationPerformer();
            result["itperf"] = itperf;
            result["regularizer"] = this->regularizationStrategy();
            result["max_iterations"] = this->iterationCountLimit();
            if( itperf ) {
                auto ddir = itperf->newtonDescentDirection();
                result["ddir"] = ddir;
                if( ddir ) {
                    result["jacobian"] = ddir->jacobianProvider();
                    result["jtrim"] = ddir->jacobianProvider()->jacobianTrimmer();
                    }
                result["linsearch"] = itperf->newtonLinearSearch();
                result["err"] = itperf->errorEstimator();
                }
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OptionalParameters::maybeSetParameter(
                        parameters, "itperf",
                        std::bind( &NewtonSolverInterface<VD>::setIterationPerformer, this, std::placeholders::_1 ) );
            auto itperf = this->iterationPerformer();
            OptionalParameters::maybeSetParameter(
                        parameters, "regularizer",
                        [this, itperf]( const std::shared_ptr< NewtonRegularizationStrategy<VD> >& rs )
                        {
                        this->setRegularizationStrategy( rs );
                        if( rs && itperf ) {
                            rs->setErrorEstimator( itperf->errorEstimator() );
                            rs->setNewtonDescentDirection( itperf->newtonDescentDirection() );
                            }
                        } );
            OptionalParameters::maybeSetParameter(
                        parameters, "max_iterations",
                        std::bind( &NewtonSolverInterface<VD>::setIterationCountLimit, this, std::placeholders::_1 ) );
            if( itperf ) {
                OptionalParameters::maybeSetParameter(
                            parameters, "ddir",
                            [this, itperf]( const std::shared_ptr< NewtonDescentDirection<VD> >& ddir )
                            {
                            if( ddir )
                                ddir->setMapping( itperf->mapping() );
                            itperf->setNewtonDescentDirection( ddir );
                            auto rs = this->regularizationStrategy();
                            if( rs )
                                rs->setNewtonDescentDirection( ddir );
                            } );
                auto ddir = itperf->newtonDescentDirection();
                if( ddir ) {
                    OptionalParameters::maybeSetParameter(
                                parameters, "jacobian",
                                std::bind( &NewtonDescentDirection<VD>::setJacobianProvider, ddir.get(), std::placeholders::_1 ) );
                    auto jp = ddir->jacobianProvider();
                    if( jp )
                        OptionalParameters::maybeSetParameter(
                                    parameters, "jtrim",
                                    std::bind( &JacobianProvider<VD>::setJacobianTrimmer, jp.get(), std::placeholders::_1 ) );
                    }

                OptionalParameters::maybeSetParameter(
                            parameters, "linsearch",
                            [itperf]( const std::shared_ptr< NewtonLinearSearch<VD> >& lineSearch )
                            {
                            lineSearch->setMapping( itperf->mapping() );
                            itperf->setNewtonLinearSearch( lineSearch );
                            } );

                OptionalParameters::maybeSetParameter(
                            parameters, "err",
                            [this, itperf]( const std::shared_ptr< ErrorEstimator<VD> >& errorEstimator )
                            {
                            itperf->setErrorEstimator( errorEstimator );
                            auto rs = this->regularizationStrategy();
                            if( rs )
                                rs->setErrorEstimator( errorEstimator );
                            } );
                }
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["itperf"] = OptionalParameters::appendNestedHelp(
                        this->iterationPerformer(),
                        "Iteration performer" );
            result["regularizer"] = OptionalParameters::appendNestedHelp(
                        this->regularizationStrategy(),
                        "Regularization strategy" );
            result["max_iterations"] = "Iteration count limit (if exceeded, exiting with the 'diverged' status)";

            if( auto itperf = this->iterationPerformer() ) {
                result["ddir"] = OptionalParameters::appendNestedHelp(
                            itperf->newtonDescentDirection(),
                            "Descent direction algorithm" );
                if( auto ddir = itperf->newtonDescentDirection() ) {
                    result["jacobian"] = OptionalParameters::appendNestedHelp(
                                ddir->jacobianProvider(),
                                "Jacobian provider" );
                    if( auto jp = ddir->jacobianProvider() )
                        result["jtrim"] = OptionalParameters::appendNestedHelp(
                                    jp->jacobianTrimmer(),
                                    "Jacobian trimmer" );
                    }
                result["linsearch"] = OptionalParameters::appendNestedHelp(
                            itperf->newtonLinearSearch(),
                            "Line search algorithm" );
                result["err"] = OptionalParameters::appendNestedHelp(
                            itperf->errorEstimator(),
                            "Error estimator" );
                }

            return result;
            }
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        NewtonSolverHolder, NewtonSolverInterface,
        newtonSolver, setNewtonSolver,
        onNewtonSolverChanged, offNewtonSolverChanged )

template< class VD >
class NewtonSolver :
    public NewtonSolverInterface<VD>,
    public FactoryMixin< NewtonSolver<VD>, NewtonSolverInterface<VD> >
    {
    public:
        typedef NewtonSolverInterface<VD> BaseClass;
        typedef typename BaseClass::Status Status;
        typedef VectorTemplate< VD > V;

        NewtonSolver() :
            m_iterationCountLimit(DefaultIterationCountLimit)
            {
            this->setComponent( std::make_shared< NewtonIterationPerformerImpl<VD> >() );
            this->setComponent( std::make_shared< SimpleErrorEstimator<VD> >() );
            this->setComponent( std::make_shared< SimpleNewtonDescentDirection<VD> >() );
            this->setComponent( std::make_shared< SimpleNewtonLinearSearch<VD> >() );
            this->setComponent( std::make_shared< DefaultJacobianProvider<VD> >() );
            }

        unsigned int iterationCountLimit() const {
            return m_iterationCountLimit;
            }

        void setIterationCountLimit( unsigned int iterationCountLimit ) {
            m_iterationCountLimit = iterationCountLimit;
            }

        void setInitialGuess( const V& initialGuess, bool hardReset = true ) {
            this->iterationPerformer()->setInitialGuess( initialGuess, hardReset );
            }

        Status run()
            {
            auto itperf = this->iterationPerformer();
            auto ddir = itperf->newtonDescentDirection();
            itperf->reset( false );
            auto hardResetTried = false;
            auto x0 = currentSolution();
            auto iterationNumberBias = 0u;
            while( true ) {
                auto breakLoop = false;
                auto iterationNumber=0u;
                for( ; !breakLoop && iterationNumber<m_iterationCountLimit; ++iterationNumber ) {
                    auto status = itperf->iteration( iterationNumber );
                    if( this->iterationObservers( iterationNumber + iterationNumberBias, this ) )
                        return BaseClass::Terminated;
                    switch( status ) {
                        case ErrorEstimator<VD>::Converged:
                            if( maybeAdjustRegularizationParameter() )
                                break;
                            ddir->reportSuccess( true );
                            return BaseClass::Converged;
                        case ErrorEstimator<VD>::ContinueIterations:
                            maybeAdjustRegularizationParameter();
                            break;
                        case ErrorEstimator<VD>::Diverged:
                            if( maybeAdjustRegularizationParameter() )
                                break;
                            ddir->reportSuccess( false );
                            if( !hardResetTried ) {
                                breakLoop = true;
                                break;
                                }
                            return BaseClass::Diverged;
                        default:
                            ASSERT( false );
                        }
                    }
                if( hardResetTried )
                    break;
                else if( ddir->hardResetMayHelp() ) {
                    setInitialGuess( x0, true );
                    hardResetTried = true;
                    iterationNumberBias += iterationNumber;
                    }
                else
                    break;
                }
            ddir->reportSuccess( false );
            return BaseClass::IterationCountLimitExceeded;
            }

        const V& currentSolution() const {
            return this->iterationPerformer()->currentSolution();
            }

    private:
        static const unsigned int DefaultIterationCountLimit = 100;
        unsigned int m_iterationCountLimit;
        bool maybeAdjustRegularizationParameter()
            {
            auto rs = this->regularizationStrategy();
            return rs ?   rs->adjustRegularizationParameter() :   false;
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ALGEBRAIC_EQN_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

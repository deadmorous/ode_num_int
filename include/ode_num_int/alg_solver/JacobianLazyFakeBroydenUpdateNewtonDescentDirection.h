// JacobianLazyFakeBroydenUpdateNewtonDescentDirection.h

#ifndef _ALGSOLVER_JACOBIANLAZYFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_JACOBIANLAZYFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "NewtonDescentDirection.h"
#include "lu/LUFactorizer.h"

namespace ctm {
namespace math {

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

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_JACOBIANLAZYFAKEBROYDENUPDATENEWTONDESCENTDIRECTION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// OdeSolverRosenbrock_W_base.h

#ifndef _ODESOLVER_ODESOLVERROSENBROCK_W_BASE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERROSENBROCK_W_BASE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolver.h"
#include "OdeEventController.h"
#include "OdeSolverJacobianTrimmer.h"
#include "OdeSolverErrorNormCalculator.h"
#include "OdeSolverStepSizeController.h"
#include "OdeSolverEventController.h"
#include "../lu/LUFactorizer.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverRosenbrock_W_base :
    public OdeSolver<VD>,
    public OdeSolverJacobianTrimmer<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeSolverRosenbrock_W_base() :
            OdeSolverJacobianTrimmer<VD>( *this, "" ),
            m_W_LU( nullptr ),
            m_hd4W( 0 )
            {}

    protected:
        template< class VDdst, class VDsrc >
        static void copy( VectorTemplate<VDdst>& dst, const VectorTemplate<VDsrc>& src )
            {
            ASSERT( dst.size() == src.size() );
            std::copy( src.begin(), src.end(), dst.begin() );
            }

        template< class VDdst, class VDsrc >
        static void copy( VectorTemplate<VDdst>&& dst, const VectorTemplate<VDsrc>& src )
            {
            ASSERT( dst.size() == src.size() );
            std::copy( src.begin(), src.end(), dst.begin() );
            }

        void linSolve( V& x, real_type hd )
            {
            sys::ScopedTimeMeasurer tm( this->tstat9 );
            m_W_LU->clearTimingStats();
            ASSERT( x.size() == m_n );
            m_J2.block( 0, 0, m_n2+m_n1, m_n2 ).mulVectRight( x.begin(), m_buf4mul.begin() );
            x.block( m_n2, m_n2+m_n1 ) += m_buf4mul.scaled( hd );
            m_W_LU->solve( &x[m_n2] );
            x.block( 0, m_n2 ) += x.block( m_n2, m_n2 ).scaled( hd );
            this->luTimingStats += m_W_LU->timingStats();
            }

        template< class VDdst, class VDsrc >
        void mult( VectorTemplate<VDdst>& dst, const VectorTemplate<VDsrc>& x )
            {
            ASSERT( dst.size() == m_n );
            ASSERT( x.size() == m_n );
            copy( dst.block(0, m_n2), x.block(m_n2, m_n2) );
            m_J1.mulVectRight( x.begin()+m_n2, dst.begin()+m_n2 );
            m_J2.block( 0, 0, m_n2+m_n1, m_n2 ).mulVectRight( x.begin(), m_buf4mul.begin() );
            dst.block( m_n2, m_n2+m_n1 ) += m_buf4mul;
            }

        void initStep(
                bool hard,
                real_type initialTime,
                const V& initialState,
                const V& initialOdeRhs,
                real_type hd )
            {
            if( hard ) {
                auto rhs = this->odeRhs();
                m_n = rhs->varCount();
                m_n2 = rhs->secondOrderVarCount();
                m_n1 = rhs->firstOrderVarCount();
                m_buf4mul.resize( m_n2+m_n1 );

                // Compute Jacobian
                buildJacobian( initialTime, initialState, initialOdeRhs );
                }

            // Compute the W matrix and its decomposition, if necessary
            updateW( hd );
            }

    private:
        typedef sparse::SparseMatrix< real_type > SparseMatrix;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData< real_type > > FastSparseMatrix;

        unsigned int m_n;               // Total number of variables
        unsigned int m_n2;              // Total number of 2nd order variables
        unsigned int m_n1;              // Total number of 1st order variables
        FastSparseMatrix m_J1;
        FastSparseMatrix m_J2;
        FastSparseMatrix m_W;           // The W matrix

        typedef std::map< double, LUFactorizer<real_type> > W_LU_map;
        W_LU_map m_W_LU_cache;
        LUFactorizer<real_type> *m_W_LU;// Decomposed trimmed W matrix
        real_type m_hd4W;               // Value of h*d corresponding to m_W_LU
        V m_buf4mul;

        void buildJacobian( real_type initialTime, const V& initialState, const V& initialOdeRhs )
            {
            auto rhs = this->odeRhs();

            real_type dx = 1e-6;
            auto makeJ = [&, this]( FastSparseMatrix& J, unsigned int istart, unsigned int isize ) {
                SparseMatrix Jm( m_n2+m_n1, m_n2+m_n1 );
                V x = initialState;
                V f( m_n );
                for( unsigned int i=istart; i<isize; ++i ) {
                    x[i] += dx;
                    rhs->rhs( f, initialTime, x );
                    x[i] = initialState[i];
                    for( unsigned int j=m_n2; j<m_n; ++j ) {
                        auto df = f[j] - initialOdeRhs[j];
                        if( df == 0 )
                            continue;
                        Jm.at( j-m_n2, i-istart ) = df / dx;
                        }
                    }
                J = Jm;
                };

            this->jacobianRefreshObservers( true );
            makeJ( m_J1, m_n2, m_n2+m_n1 );
            makeJ( m_J2, 0, m_n2 );
            this->jacobianRefreshObservers( false );

            // Trim m_W
            SparseMatrix m( m_n2+m_n1, m_n2+m_n1 );
            m.addIdentity();
            m += m_J1;
            m += m_J2;
            m_W = m;

            // Clear cache for W
            m_W_LU_cache.clear();
            m_W_LU = nullptr;
            m_hd4W = 0;
            }

        void updateW( real_type hd )
            {
            ASSERT( hd != 0 ); // Required for cache to work
            if( m_hd4W != hd ) {
                m_hd4W = hd;
                auto it = m_W_LU_cache.find( hd );
                if( it != m_W_LU_cache.end() )
                    m_W_LU = &it->second;
                else {
                    const size_t MaxCacheSize = 100;
                    if( m_W_LU_cache.size() >= MaxCacheSize )
                        m_W_LU_cache.clear();
                    m_W.makeZero();
                    m_W.addIdentity();
                    m_W -= m_J1.scaled( hd );
                    m_W -= m_J2.scaled( hd*hd );
                    auto itbool = m_W_LU_cache.insert( typename W_LU_map::value_type( hd, LUFactorizer<real_type>(m_W) ) );
                    ASSERT( itbool.second );
                    m_W_LU = &itbool.first->second;
                    }
                }
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESOLVERROSENBROCK_W_BASE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

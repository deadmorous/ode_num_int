// LUFactorizer.h

#ifndef _LU_LUFACTORIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _LU_LUFACTORIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./LUFactorizerTimingStats.h"
#include "../infra/cxx_zeroinit.h"

// #define CTM_MATH_LU_USE_MX_FAST_CACHE

namespace ctm {
namespace math {

template< class RealType >
class LUFactorizer
    {
    public:
        typedef RealType real_type;
        LUFactorizer() : m_factorized(false) {}

        bool empty() const {
            return m_p.empty();
            }

        template< class Container >
        explicit LUFactorizer( const Container& matrix ) {
            setMatrix( matrix );
            }

        template< class It >
        LUFactorizer( It matrixBegin, It matrixEnd ) {
            setMatrix( matrixBegin, matrixEnd );
            }

        template< class It >
        void setMatrix( It matrixBegin, It matrixEnd )
            {
            sys::ScopedTimeMeasurer tm( m_timingStats.setMatrixTiming );
            // Determine matrix size
            unsigned int n = [&, this]() -> unsigned int {
                unsigned int nr = 0,   nc = 0;
                for( auto it=matrixBegin; it!=matrixEnd; ++it ) {
                    nr = std::max( nr, it->first.first + 1 );
                    nc = std::max( nc, it->first.second + 1 );
                    }
                if( nr != nc )
                    throw cxx::exception("Invalid matrix stencil: matrix is not square");
                return nr;
                }();

            // Allocate and initialize storage for m_p, m_q, m_s
            auto fill = []( std::vector<unsigned int>& v, unsigned int size, unsigned int value ) {
                v.resize( size );
                std::fill( v.begin(), v.end(), value );
                };
            auto setMin = []( unsigned int& acc, unsigned int value ) {
                if( acc > value )
                    acc = value;
                };
            auto setMax = []( unsigned int& acc, unsigned int value ) {
                if( acc < value )
                    acc = value;
                };
            fill( m_p, n, n );
            fill( m_q, n, n );
            fill( m_s, n, 0 );
            for( auto it=matrixBegin; it!=matrixEnd; ++it ) {
                auto r = it->first.first;
                auto c = it->first.second;
                setMin( m_p[r], c );
                setMin( m_q[c], r );
                setMax( m_s[r], c );
                }

            // Verify m_p and m_q
            for( unsigned int i=0; i<n; ++i ) {
                if( m_p[i] >= n   ||   m_q[i] >= n )
                    throw cxx::exception("Invalid matrix stencil: the stencil is incomplete");
                if( m_p[i] > i   ||   m_q[i] > i )
                    throw cxx::exception("Invalid matrix stencil: the stencil does not cover the diagonal");
                }

            // Compute address sequences for triangular matrices m_l, m_u
            auto initTriangularMatrix = [n](
                    std::vector<real_type>& m,
                    std::vector<unsigned int>& a,
                    const std::vector<unsigned int>& p,
                    bool hasDiagonal ) {
                a.resize( n + 1 );
                a[0] = 0;
                unsigned int d = hasDiagonal? 1: 0;
                for( unsigned int i=0; i<n; ++i )
                    a[i+1] = a[i] + ( i-p[i] ) + d;
                m.resize( a[n] );
                std::fill( m.begin(), m.end(), real_type() );
                };
            initTriangularMatrix( m_l, m_al, m_p, false );
            initTriangularMatrix( m_u, m_au, m_q, true );

            // Fill matrices m_al, m_au with the contents of A
            for( auto it=matrixBegin; it!=matrixEnd; ++it ) {
                auto r = it->first.first;
                auto c = it->first.second;
                auto value = it->second;
                if( r <= c )
                    U( r, c ) = value;
                else
                    L( r, c ) = value;
                }

            m_factorized = false;
#ifdef CTM_MATH_LU_USE_MX_FAST_CACHE
            m_mxFastCache.clear();
#endif // CTM_MATH_LU_USE_MX_FAST_CACHE
            }

        template< class Container >
        void setMatrix( const Container& matrix ) {
            setMatrix( matrix.begin(), matrix.end() );
            }

        template< class It >
        void setMatrixFast( It matrixBegin, It matrixEnd )
            {
            sys::ScopedTimeMeasurer tm( m_timingStats.setMatrixFastTiming );
            std::fill( m_l.begin(), m_l.end(), 0 );
            std::fill( m_u.begin(), m_u.end(), 0 );
#ifdef CTM_MATH_LU_USE_MX_FAST_CACHE
            auto fc = mxFastCache( matrixBegin, matrixEnd );
            for( auto it=matrixBegin; it!=matrixEnd; ++it, ++fc )
                **fc = it->second;
#else // CTM_MATH_LU_USE_MX_FAST_CACHE
            auto elementsSet = 0;
            for( auto it=matrixBegin; it!=matrixEnd; ++it ) {
                ++elementsSet;
                auto r = it->first.first;
                auto c = it->first.second;
                if (!setLU(r, c, it->second)) {
                    // Fall back to setMatrix because the sparsity layout has changed
                    setMatrix(matrixBegin, matrixEnd);
                    return;
                    }
                }
#endif // CTM_MATH_LU_USE_MX_FAST_CACHE
            m_factorized = false;
            }

        template< class Container >
        void setMatrixFast( const Container& matrix ) {
            setMatrixFast( matrix.begin(), matrix.end() );
            }

        void solve( real_type *rhs )
            {
            factorize();
            sys::ScopedTimeMeasurer tm( m_timingStats.solveTiming );
            unsigned int n = m_p.size();

            // Forward iteration
            for( unsigned int i=1; i<n; ++i ) {
                auto& x = rhs[i];
                auto adr_L_i_0 = L_column_0_address( i );
                for( unsigned int k=m_p[i]; k<i; ++k )
                    x -= adr_L_i_0[k] * rhs[k];
                }

            // Backward iteration
            for( unsigned int i=n-1; i!=~0; --i ) {
                auto& x = rhs[i];
                for( unsigned int k=i+1; k<=m_s[i]; ++k )
                    if( i >= m_q[k] )
                        x -= U(i,k) * rhs[k];
                x /= U(i,i);
                }
            }

        bool isFactorized() const {
            return m_factorized;
            }

        unsigned int size() const {
            return m_p.size();
            }

        real_type elementAt( unsigned int r, unsigned int c ) const
            {
            ASSERT( r < m_p.size() );
            ASSERT( c < m_p.size() );
            if( m_factorized ) {
                if( r > c && c >= m_p[r]   ||   r <= c && r >= m_q[c] ) {
                    real_type result = 0;
                    auto kmin = std::max(m_p[r], m_q[c]);
                    auto kmax = std::min(r, c);
                    for (auto k=kmin; k<=kmax; ++k )
                        result += safeL(r,k)*U(k,c);
                    return result;
                    }
                }
            else {
                if( r > c ) {
                    if( c >= m_p[r] )
                        return m_l[m_al[r] + (c-m_p[r])];
                    }
                else {
                    if( r >= m_q[c] )
                        return m_u[m_au[c] + (r-m_q[c])];
                    }
                }
            return 0;
            }

        unsigned int firstCol( unsigned int r ) const
            {
            ASSERT( r < m_p.size() );
            return m_p[r];
            }

        unsigned int firstRow( unsigned int c ) const
            {
            ASSERT( c < m_q.size() );
            return m_q[c];
            }

        unsigned int lastCol( unsigned int r ) const
            {
            ASSERT( r < m_s.size() );
            return m_s[r];
            }

        // Implements LU update to satisfy secant condition LU*s=y (e.g. for quasi-Newton method).
        // The point is that the sparsity pattern of L and U is preserved.
        // According to the following article:
        // W.E. Hart, F. Soesianto
        // On the solution of highly structured nonlinear equations
        // Journal of Computational and Applied Mathematics
        // Volume 40, Issue 3, 24 July 1992, Pages 285-296
        // http://www.sciencedirect.com/science/article/pii/037704279290184Y
        void secantUpdateHart( const real_type *s, const real_type *y )
            {
            factorize();
            sys::ScopedTimeMeasurer tm( m_timingStats.updateTiming );
            unsigned int n = m_p.size();
            m_r.resize( n );
            for( unsigned int i=0; i<n; ++i ) {
                real_type beta = y[i];
                real_type aa = 0;
                for( unsigned int k=m_p[i]; k<i; ++k ) {
                    const auto& rk = m_r[k];
                    beta -= L(i,k)*rk;
                    aa += rk*rk;
                    }
                auto umaxcol = m_s[i];
                for( unsigned int k=i; k<=umaxcol; ++k )
                    if( i >= m_q[k] ) {
                        const auto& sk = s[k];
                        beta -= U(i,k)*sk;
                        aa += sk*sk;
                        }
                auto& ri = m_r[i];
                ri = 0;
                if( beta == 0 ) {
                    // Just compute r[i]
                    for( unsigned int k=i; k<=umaxcol; ++k )
                        if( i >= m_q[k] )
                            ri += U(i,k) * s[k];
                    }
                else {
                    // Update L, U; compute r[i]
                    ASSERT( aa > 0 );
                    auto factor = beta/aa;
                    for( unsigned int k=m_p[i]; k<i; ++k )
                        L(i,k) += m_r[k] * factor;
                    for( unsigned int k=i; k<=umaxcol; ++k )
                        if( i >= m_q[k] ) {
                            const auto& sk = s[k];
                            auto& Uik = U(i,k);
                            Uik += sk*factor;
                            ri += Uik * sk;
                            }
                    }
                }
            }

        void secantUpdateProportional( const real_type *s, const real_type *y )
            {
            factorize();
            sys::ScopedTimeMeasurer tm( m_timingStats.updateTiming );
            unsigned int n = m_p.size();
            m_r.resize( n );
            for( unsigned int i=0; i<n; ++i ) {
                real_type beta = y[i];
                real_type cl = 0;
                for( unsigned int k=m_p[i]; k<i; ++k )
                    cl += L(i,k)*m_r[k];
                auto umaxcol = m_s[i];
                real_type cu = 0;
                for( unsigned int k=i; k<=umaxcol; ++k )
                    if( i >= m_q[k] )
                        cu += U(i,k)*s[k];
                real_type c = cl + cu;
                if( beta == 0 )
                    // Just compute r[i]
                    m_r[i] = cu;
                else if( c == 0 )
                    throw cxx::exception( "LUFactorizer::secantUpdateProportional() failed due to unexpected orthogonality");
                else {
                    // Update L, U; compute r[i]
                    auto factor = beta/c;
                    /*
                    const real_type Threshold = 0.5;
                    if( factor < -Threshold )
                        factor = -Threshold;
                    else if( factor > Threshold )
                        factor = Threshold;
                        */
                    factor += 1;
                    for( unsigned int k=m_p[i]; k<i; ++k )
                        L(i,k) *= factor;
                    auto& ri = m_r[i];
                    ri = 0;
                    for( unsigned int k=i; k<=umaxcol; ++k )
                        if( i >= m_q[k] ) {
                            auto& Uik = U(i,k);
                            Uik *= factor;
                            ri += Uik * s[k];
                            }
                    }
                }
            }

        typedef LUFactorizerTimingStats TimingStats;

        TimingStats timingStats() const { return m_timingStats; }
        void clearTimingStats() {
            m_timingStats = TimingStats();
            }

    private:
        real_type& L( unsigned int r, unsigned int c )
            {
            ASSERT( r > c );
            ASSERT( c >= m_p[r] );
            return m_l[m_al[r] + (c-m_p[r])];
            }

        bool setL( unsigned int r, unsigned int c, double x )
            {
            if (r > c && c >= m_p[r]) {
                m_l[m_al[r] + (c-m_p[r])] = x;
                return true;
                }
            else
                return false;
            }

        // Don't dereference the pointer returned, element at column 0 will most likely be outside the sparsity pattern!
        real_type *L_column_0_address( unsigned int r )
            {
                return m_l.data() + m_al[r] - m_p[r];
            }

        real_type safeL( unsigned int r, unsigned int c ) const {
            return r == c ?    1 :    const_cast<LUFactorizer*>(this)->L( r, c );
            }

        real_type& U( unsigned int r, unsigned int c )
            {
            ASSERT( r <= c );
            ASSERT( r >= m_q[c] );
            return m_u[m_au[c] + (r-m_q[c])];
            }

        bool setU( unsigned int r, unsigned int c, double x )
            {
            if (r <= c && r >= m_q[c]) {
                m_u[m_au[c] + (r-m_q[c])] = x;
                return true;
                }
            else
                return false;
            }

        // Don't dereference the pointer returned, element at row 0 will most likely be outside the sparsity pattern!
        real_type *U_row_0_address( unsigned int c ) {
            return m_u.data() + m_au[c] - m_q[c];
            }

        real_type U( unsigned int r, unsigned int c ) const
            {
            ASSERT( r <= c );
            ASSERT( r >= m_q[c] );
            return m_u[m_au[c] + (r-m_q[c])];
            }

        bool setLU( unsigned int r, unsigned int c, double x ) {
            return c < r? setL(r, c, x): setU(r, c, x);
            }

        unsigned int maxpq( unsigned int r, unsigned int c ) {
            return std::max( m_p[r], m_q[c] );
            }

        void factorize()
            {
            if( m_factorized )
                return;
            sys::ScopedTimeMeasurer tm( m_timingStats.factorizeTiming );
            m_factorized = true;
            unsigned int n = m_p.size();
            for( unsigned int i=0; i<n; ++i ) {
                // Compute L row
                auto adr_L_i_0 = L_column_0_address( i );
                for( unsigned int c=m_p[i]; c<i; ++c ) {
                    auto& l = adr_L_i_0[c];
                    auto k = maxpq( i, c );
                    auto prow = adr_L_i_0 + k;
                    auto pcol = &U( k, c );
                    for( ; k<c; ++k, ++prow, ++pcol )
                        l -= *prow * *pcol; //  l -= L(i,k) * U(k,c);
                    l /= *pcol; // l /= U(c,c);
                    }

                // Compute U column
                auto adr_U_0_i = U_row_0_address( i );
                for( unsigned int r=m_q[i]; r<i; ++r ) {
                    auto& u = adr_U_0_i[r];
                    auto k = maxpq( r, i );
                    auto prow = L_column_0_address( r ) + k;
                    auto pcol = adr_U_0_i + k;
                    for( ; k<r; ++k, ++prow, ++pcol )
                        u -= *prow * *pcol; //  u -= L(r,k) * U(k,i);
                    }

                // Compute diagonal element
                auto& d = adr_U_0_i[i]; // == U(i, i);
                for( unsigned int k=maxpq(i,i); k<i; ++k )
                    d -= adr_L_i_0[k] * adr_U_0_i[k]; // d -= safeL(i,k) * U(k,i);
                }
            }

        std::vector< unsigned int > m_p;    // A(i,j) == 0 if j < m_p[i]
        std::vector< unsigned int > m_q;    // A(i,j) == 0 if i < m_q[j]
        std::vector< unsigned int > m_s;    // U(i,j) == 0 if j > s[i]
        std::vector< unsigned int > m_al;   // Address sequence for L
        std::vector< unsigned int > m_au;   // Address sequence for U
        std::vector< real_type > m_l;       // L
        std::vector< real_type > m_u;       // U
        std::vector< real_type > m_r;       // r for use in secantUpdateHart()
        cxx::bool0 m_factorized;
        TimingStats m_timingStats;

#ifdef CTM_MATH_LU_USE_MX_FAST_CACHE
        std::vector< real_type* > m_mxFastCache;    // Index = ordinal number of element in sparse matrix, value = destination address in L or U
        template< class It >
        real_type* const* mxFastCache( It matrixBegin, It matrixEnd )
            {
            if( m_mxFastCache.empty() )
                for( auto it=matrixBegin; it!=matrixEnd; ++it ) {
                    auto r = it->first.first;
                    auto c = it->first.second;
                    m_mxFastCache.push_back( c < r ?   &L(r, c) :   &U(r, c) );
                    }
            return m_mxFastCache.data();
            }
#endif // CTM_MATH_LU_USE_MX_FAST_CACHE
    };

} // end namespace math
} // end namespace ctm

#endif // _LU_LUFACTORIZER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

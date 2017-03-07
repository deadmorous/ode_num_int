// SparseJacobianCalculator.h

#ifndef _ALGSOLVER_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALGSOLVER_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../la/sparse_util.h"
#include "./interfaces/VectorMapping.h"
#include <set>
#include <algorithm>

namespace ctm {
namespace math {

template< class VD >
class SparseJacobianCalculator
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;
        typedef VectorMapping<VD> Mapping;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > SparseMatrix;
        typedef typename SparseMatrix::Index Index;

        explicit SparseJacobianCalculator( SparseMatrix *J ) :
            m_J( J )
            {
            ASSERT( J );
            using namespace std;

            // Initialize the set containing columns to be processed
            auto rows = J->size().first;
            auto cols = J->size().second;
            set<unsigned int> colsToPass;
            for( auto col=0u; col<cols; ++col )
                colsToPass.insert(col);

            // Compute the numbers of nonzero elements in each column
            vector<unsigned int> colSizes( cols, 0 );
            for( const auto& e : *J )
                ++colSizes[e.first.second];

            // Compute column layouts (for each column, find the
            // numbers and element addresses of all rows containing nonzero elements)
            vector< vector<RP> > colRows( cols );
            for( auto col=0u; col<cols; ++col ) {
                colRows[col].resize( colSizes[col] );
                colSizes[col] = 0;
                }
            for( auto& e : *J ) {
                auto col = e.first.second;
                colRows[col][colSizes[col]++] = RP( e.first.first, &e.second );
                }

            auto addRows = [&colRows]( vector<bool>& affectedRows, unsigned int col ) {
                for( const auto& rp : colRows[col] )
                    affectedRows[rp.first] = true;
                };

            auto intersects = [&colRows]( const vector<bool>& affectedRows, unsigned int col ) {
                for( const auto& rp : colRows[col] )
                    if( affectedRows[rp.first] )
                        return true;
                return false;
                };

            while( !colsToPass.empty() ) {
                m_layout.push_back( MulticolEvalData() );
                auto& multicolData = *m_layout.rbegin();
                auto col = *colsToPass.begin();
                colsToPass.erase( colsToPass.begin() );
                vector<bool> affectedRows( rows, false );
                addRows( affectedRows, col );
                multicolData.push_back( C2R( col, colRows[col] ) );
                for( auto itcol=colsToPass.begin(); itcol!=colsToPass.end(); ) {
                    auto col2 = *itcol;
                    if( intersects( affectedRows, col2 ) )
                        ++itcol;
                    else {
                        multicolData.push_back( C2R( col2, colRows[col2] ) );
                        addRows( affectedRows, col2 );
                        itcol = colsToPass.erase( itcol );
                        }
                    }
                }
            }

        void calculate( const VectorMapping<VD>& mapping, const V& x0, real_type delta ) const
            {
            auto n = mapping.inputSize();
            ASSERT( m_J->size().first == mapping.outputSize() );
            ASSERT( m_J->size().second == n );
            V x(n), f0(n), f(n);
            mapping.map( f0, x0 );
            for( const auto& multicolData : m_layout ) {
                x = x0;
                for( const auto& c2r : multicolData )
                    x[c2r.first] += delta;
                mapping.map( f, x );
                for( const auto& c2r : multicolData ) {
                    for( const auto& rp : c2r.second )
                        *rp.second = ( f[rp.first] - f0[rp.first] ) / delta;
                    }
                }
            }

        unsigned int mappingEvaluationsRequired() const {
            return m_layout.size() + 1;
            }

    private:
        SparseMatrix *m_J;
        // First element is a row number,
        // second element is the address of element in the Jacobian
        typedef std::pair< unsigned int, real_type* > RP;
        // First element is a column number,
        // second element is the set of potentially nonzero Jacobian rows in that column
        typedef std::pair<unsigned int, std::vector<RP> > C2R;

        // Each mapping evaluation could be used for computing more than one column of the Jacobian,
        // if its sparse structure allows it. In this array, there is information about all columns
        // to be computed within a single mapping evaluation.
        typedef std::vector< C2R > MulticolEvalData;

        // Information about how to compute all columns of the Jacobian, grouped by multi-column
        // data
        typedef std::vector< MulticolEvalData > JacobianEvalData;

        JacobianEvalData m_layout;
    };

template< class VD >
inline void computeSparseJacobian(
        sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData< typename VD::value_type > >& J,
        const VectorMapping<VD>& mapping,
        const VectorTemplate<VD>& x0, typename VD::value_type delta )
    {
    SparseJacobianCalculator<VD>( &J ).calculate( mapping, x0, delta );
    }

} // end namespace math
} // end namespace ctm

#endif // _ALGSOLVER_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

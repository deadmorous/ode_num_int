// test_sparse_jacobian.cpp

#include "ode_num_int/SparseJacobianCalculator.h"
#include "ode_num_int/computeJacobian.h"
#include <iostream>

namespace {

using namespace std;
using namespace ctm;
using namespace math;

typedef VectorData<double> VD;
typedef VectorTemplate<VD> V;
typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<double> > FastSparseMatrix;
typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixData<double> > SparseMatrix;

struct M : public VectorMapping< VD >
    {
    public:
        explicit M( unsigned int n ) : m_n(n)
            {
            ASSERT( n > 1 );
            SparseMatrix m( n, n );
            m.addScaledIdentity( 10 );
            m.block( 1, 0, n-1, n-1 ).addScaledIdentity( 1 );
            m.block( 0, 1, n-1, n-1 ).addScaledIdentity( 2 );
            m.block( 0, n-1, 1, 1 ).addScaledIdentity( 10 );
            m.block( 0, n-2, 2, 2 ).addScaledIdentity( 1 );
            m.block( n-1, 0, 1, 1 ).addScaledIdentity( 10 );
            m.block( n-2, 0, 2, 2 ).addScaledIdentity( 2 );
            m_matrix = m;
            }

        virtual unsigned int inputSize() const {
            return m_n;
            }

        virtual unsigned int outputSize() const {
            return m_n;
            }

        virtual void map( V& dst, const V& x ) const {
            dst = m_matrix * x;
            }
    private:
        unsigned int m_n;
        FastSparseMatrix m_matrix;
    };

void testSparseJacobian()
    {
    unsigned int n = 20;
    M mapping( n );
    V x0( n );
    auto Jslow = computeJacobian( mapping, x0 );
    FastSparseMatrix J;
    J = Jslow;
    J.makeZero();
    computeSparseJacobian( J, mapping, x0, 1e-6 );

    using T1 = decltype (Jslow);
    using T2 = decltype (J);
    ASSERT( !std::equal(Jslow.begin(), Jslow.end(), J.begin(), [](T1::iterator::value_type a, T2::iterator::value_type b) {
                return a.first == b.first && a.second == b.second;
    } ) );

    // TODO: Remove
    for (auto item : J) {
        cout << item.first.first << " === " << item.first.second << " --- " << item.second << endl;
    }

    cout << J;
    }

// Uncomment to call the test at startup
// struct _{_() { testSparseJacobian(); } }__;

} // anonymous namespace

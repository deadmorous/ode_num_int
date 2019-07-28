#include <gtest/gtest.h>
#include <iostream>
#include "ode_num_int/SparseJacobianCalculator.h"
#include "ode_num_int/computeJacobian.h"


using namespace ctm::math;

typedef VectorData<double> VD;
typedef VectorTemplate<VD> V;
typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<double> > FastSparseMatrix;
typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixData<double> > SparseMatrix;
typedef typename FastSparseMatrix::Data::iterator iterator;
typedef typename FastSparseMatrix::Data::const_iterator const_iterator;

struct M : public VectorMapping< VD >
    {
    public:
        explicit M( unsigned int n ) : m_n(n)
            {
            ASSERT( n > 1 );
            m_matrix = matrix(n);
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

        static SparseMatrix matrix(unsigned int n) {
            SparseMatrix m( n, n );
            m.addScaledIdentity( 10 );
            m.block( 1, 0, n-1, n-1 ).addScaledIdentity( rand() );
            m.block( 0, 1, n-1, n-1 ).addScaledIdentity( rand() );
            m.block( 0, n-1, 1, 1 ).addScaledIdentity( rand() );
            m.block( 0, n-2, 2, 2 ).addScaledIdentity( rand() );
            m.block( n-1, 0, 1, 1 ).addScaledIdentity( rand() );
            m.block( n-2, 0, 2, 2 ).addScaledIdentity( rand() );
            return m;
        }

        FastSparseMatrix::iterator begin()
        {
            return m_matrix.begin();
        }

        FastSparseMatrix::iterator end()
        {
            return m_matrix.end();
        }


    private:
        unsigned int m_n;
        FastSparseMatrix m_matrix;
    };

TEST(SparseJacobianCalculator, CalculationTest)
{
    unsigned int n = 20;
    M mapping( n );
    V x0( n );
    auto Jslow = computeJacobian( mapping, x0 );
    FastSparseMatrix J;
    J = Jslow;
    J.makeZero();
    // TODO: Compare J and M::matrix(n)
    computeSparseJacobian( J, mapping, x0, 1e-6 );

    using T1 = decltype (Jslow);
    using T2 = decltype (J);

    M mappingTest( n );

    ASSERT_TRUE(std::equal(Jslow.begin(), Jslow.end(), J.begin(), [](T1::iterator::value_type a, T2::iterator::value_type b) {
               return a.first == b.first && a.second == b.second;
    } ) );

    ASSERT_TRUE(std::equal(Jslow.begin(), Jslow.end(), mapping.begin(), [](T1::iterator::value_type a, T2::iterator::value_type b) {
               return a.first == b.first && a.second == b.second;
    } ) );

    ASSERT_FALSE(std::equal(Jslow.begin(), Jslow.end(), mappingTest.begin(), [](T1::iterator::value_type a, T2::iterator::value_type b) {
               return a.first == b.first && a.second == b.second;
    } ) );

}


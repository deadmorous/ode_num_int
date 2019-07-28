#include <gtest/gtest.h>
#include "ode_num_int/OdeSolver.h"
#include "ode_num_int/OdeSolverEventController.h"
#include "ode_num_int/JacobianWidthBasedTrimmer.h"

using namespace ctm::math;

//TEST(OdeSolverEventController, OdeSolverEventControllerTest)
//{
//    OdeSolverEventController<VectorData<double>> A;


//}

//struct Matrix : public VectorMapping<VectorData<double>>
//{
//public:
//    explicit Matrix (unsigned int n) : m_n(n)
//    {
//    sparse::SparseMatrixTemplate< sparse::SparseMatrixData<double>> m(n);
//    m.addScaledIdentity( 10 );
//    m.block( 1, 0, n-1, n-1 ).addScaledIdentity( rand() );
//    m.block( 0, 1, n-1, n-1 ).addScaledIdentity( rand() );
//    m.block( 0, n-1, 1, 1 ).addScaledIdentity( rand() );
//    m.block( 0, n-2, 2, 2 ).addScaledIdentity( rand() );
//    m.block( n-1, 0, 1, 1 ).addScaledIdentity( rand() );
//    m.block( n-2, 0, 2, 2 ).addScaledIdentity( rand() );
//    }

//    virtual unsigned int inputSize() const {return m_n;}
//    virtual unsigned int outputSize() const {return m_n;}
//    virtual void map(VectorTemplate<VectorData<double>>& dst, const VectorTemplate<VectorData<double>>& x)
//    const { dst = m_matrix * x;}
//private:
//    unsigned int m_n;
//    sparse::SparseMatrixTemplate< sparse::SparseMatrixData<double>> m_matrix;
//};


//TEST (JacobianWidthBasedTrimmer, )
//{
//    unsigned int n = 20;
//    Matrix J(n);
//    JacobianWidthBasedTrimmer<VectorData<double>> A;
//    A.trimJacobian(J);
//}

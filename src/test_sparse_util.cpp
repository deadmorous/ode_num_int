// test_SparseMatrixTemplate.cpp

#include "ode_num_int/la/SparseMatrixTemplate.h"

#include <iostream>

using namespace std;
using namespace ctm;
using namespace math;
using namespace sparse;

namespace {

void testSparseUtil()
    {
    typedef SparseMatrix<double> SM;
    typedef SM::Index I;
    SM m(3, 3);
    m.addScaledIdentity( 1 );
    m.at(0, 0) += 1;
    m.block( 0, 0,   2, 2 ).addScaledIdentity( 1 );
    m.block(I(2,2), I(1,1)) += m.block(I(0,0), I(1,1)).scaled( 3 );
    m.block(I(2,2), I(1,1)) += m.block(I(1,1), I(1,1));

    cout << m;

    SM m2 = m.block(1, 1, 2, 2);
    // m2.block(I(1,1), I(1,1)).clear();
    // m2.resizeFromData();
    m2.at(1, 0) = 1;
    cout << m2;
    cout << m2.transposed();
    cout << m2*m2.transposed();
    cout << m2.transposed().clone()*m2;

    Vector< double > x( VectorData< double >( vector<double>{ 1, 2 } ) );
    cout << x << endl;
    auto m2x = m2 * x;
    cout << m2x << endl;
    auto xm2 = x * m2;
    cout << xm2 << endl;

    // m2 *= m2;
    cout << m;
    cout << m2;
    m.block( 1, 1, 2, 2 ) *= m2.scaled( 2 ) * m2;
    cout << m;

    // SM m3( 7, 7 );
    // m3.addScaledIdentity( 2 );
    auto m3 = SM::scaledIdentity(7, 2);
    m3.block(1,0, 6,6).addIdentity();
    m3.block(0,1, 6,6).addIdentity();
    cout << m3*m3;
    }

// Uncomment to call the test at startup
// struct _{_() { testSparseUtil(); } }__;

} // anonymous namespace

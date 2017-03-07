// test_lu_fact.cpp

#include "ode_num_int/la/SparseMatrixTemplate.h"
#include "ode_num_int/LUFactorizer.h"
#include <cstdlib>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace ctm;
using namespace math;
using namespace sparse;

namespace {

void testLUfact()
    {
    unsigned int n = 10,    nsamples = 10000;

    SparseMatrix<double> m(n, n);
    Vector<double> rhs(n);
    for( unsigned int i=0; i<nsamples; ++i ) {
        m.at( rand()%n, rand()%n ) = static_cast<double>(rand()) / RAND_MAX;
        rhs[rand()%n] = static_cast<double>(rand()) / RAND_MAX;
        }

    Vector<double> x = rhs;
    LUFactorizer<double>( m ).solve( x.data().data() );

    auto r = m * x;

    double xmax = 0,   dmax = 0;
    for( unsigned int i=0; i<n; ++i ) {
        xmax = max( xmax, fabs(rhs[i]) );
        dmax = max( dmax, fabs(rhs[i]-r[i]) );
        }
    double relError = dmax / xmax;
    if( relError > 1e-10 )
        throw cxx::exception( "testLUfact() failed" );
    }

// Uncomment to call the test at startup
// struct _{_() { testLUfact(); } }__;

} // anonymous namespace

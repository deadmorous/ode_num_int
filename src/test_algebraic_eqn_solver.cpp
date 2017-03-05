// test_algebraic_eqn_solver.cpp

#include "algebraic_eqn_solver.h"
#include "alg_solver/MappingNormalizer.h"
#include "alg_solver/SimpleNewtonDescentDirection.h"

#include <iostream>

using namespace std;
using namespace ctm;
using namespace math;

namespace {

typedef VectorData<double> MyVD;
typedef VectorTemplate< MyVD > MyVec;

class MyEquation1d_quadratic : public VectorMapping< MyVD >
    {
    public:
        unsigned int inputSize() const { return 1; }
        unsigned int outputSize() const { return 1; }

        void map( V& dst, const V& x ) const
            {
            vectorMappingPreObservers( x, this );
            dst.resize( 1 );
            dst[0] = ( x[0] - 3 )*( x[0] + 1 );
            vectorMappingPostObservers( x, dst, this );
            }
    };

class MyEquation1d_degenerate : public VectorMapping< MyVD >
    {
    public:
        unsigned int inputSize() const { return 1; }
        unsigned int outputSize() const { return 1; }

        void map( V& dst, const V& x ) const
            {
            vectorMappingPreObservers( x, this );
            dst.resize( 1 );
            dst[0] = ( x[0] - 1 )*( x[0] - 1 );
            vectorMappingPostObservers( x, dst, this );
            }
    };

class MyEquation2d_quadratic : public VectorMapping< MyVD >
    {
    public:
        unsigned int inputSize() const { return 2; }
        unsigned int outputSize() const { return 2; }

        void map( V& dst, const V& x ) const
            {
            vectorMappingPreObservers( x, this );
            dst.resize( 2 );
            dst[0] = ( x[0] - 3 )*( x[0] + 1 );
            dst[1] = 0.5*( x[1] + 1 )*( x[1] - 5 );
            vectorMappingPostObservers( x, dst, this );
            }
    };

class MyEquation2d_degenerate : public VectorMapping< MyVD >
    {
    public:
        unsigned int inputSize() const { return 2; }
        unsigned int outputSize() const { return 2; }

        void map( V& dst, const V& x ) const
            {
            vectorMappingPreObservers( x, this );
            dst.resize( 2 );
            dst[0] = ( x[0] - 1 )*( x[0] - 1 );
            dst[1] = 0.5*( x[1] - 2 )*( x[1] - 2 );
            vectorMappingPostObservers( x, dst, this );
            }
    };

class CoupledNonlinearOscillators : public VectorMapping< MyVD >
    {
    public:
        explicit CoupledNonlinearOscillators( unsigned int count = 10, real_type c0 = 1e3, real_type c2 = 1e3, real_type f = 100 ) :
            m_count( count ),
            m_c0( c0 ),
            m_c2( c2 ),
            m_f( f )
            {}
        unsigned int inputSize() const { return m_count; }
        unsigned int outputSize() const { return m_count; }

        void map( V& dst, const V& x ) const
            {
            vectorMappingPreObservers( x, this );
            dst.resize( m_count );
            for( unsigned int i=0; i<m_count; ++i ) {
                real_type dprev = x[i];
                if( i > 0 )
                    dprev -= x[i-1];
                real_type f = -force( dprev );
                if( i+1 < m_count )
                    f += force( x[i+1] - x[i] );
                else if( i+1 == m_count )
                    f += m_f;
                dst[i] = f;
                }
            vectorMappingPostObservers( x, dst, this );
            }

    private:
        unsigned int m_count;
        real_type m_c0;
        real_type m_c2;
        real_type m_f;
        real_type force( real_type deformation ) const {
            return deformation * ( m_c0 + m_c2*deformation*deformation );
            }
    };

void testAlgebraicEqnSolver()
    {
    // Specify callbacks
    auto cb = []( unsigned int iterationNumber, const NewtonSolverInterface<MyVD>* solver ) -> bool
        {
        auto itperf = solver->iterationPerformer();
        auto alpha = itperf->currentStepRatio();
        auto x = itperf->currentSolution();
        auto statusMsg = itperf->errorEstimator()->currentStatusMessage();
        cout << "Iteration " << ( iterationNumber + 1 )
             << ", alpha=" << alpha
             << ", x=" << x.toString(3)
             << ", " << statusMsg << endl;

        auto res = itperf->mapping()->map( x );
        return false;
        };

    auto ddircb = []( const sparse::SparseMatrixD& J, const MyVec& x0, const MyVec& f0 ) {
        cout << "Jacobian:" << endl;
        cout << J;
        };

    auto errcb = []( const MyVec& x, const MyVec& res, const ErrorEstimator<MyVD> *ee ) {
        cout << "solution=" << x << endl
             << "residual=" << res << endl;
        };

    auto lscb = []( unsigned int iteration, double alpha, const MyVec& dir, const MyVec& x0, const MyVec& f0, const MyVec& f ) {
        cout << "LineSearch: "
             << "iteration=" << (iteration+1)
             << ", alpha=" << alpha
             << ", |f|/|f0|=" << f.infNorm() / f0.infNorm() << endl;
        };

    // Define solver
    NewtonSolver<MyVD> solver;
    auto eqnRhs = make_shared< CoupledNonlinearOscillators >( 10, 1e3, 1e4, 100 );
    auto eqnRhsScaled = make_shared< MappingNormalizer<MyVD> >();
    eqnRhsScaled->setMapping( eqnRhs );
    solver.setComponent( eqnRhsScaled );

    // Set up descent direction implementation
    auto ddir = make_shared< SimpleNewtonDescentDirection<MyVD> >();
//    auto ddir = make_shared< JacobianBroydenUpdateNewtonDescentDirection<MyVD> >();
//    auto ddir = make_shared< JacobianFakeBroydenUpdateNewtonDescentDirection<MyVD> >();
//    auto ddir = make_shared< JacobianHartUpdateNewtonDescentDirection<MyVD> >();
//    auto ddir = make_shared< ConstJacobianNewtonDescentDirection<MyVD> >();
    solver.setComponent( ddir );

    // Set up observers
    auto scopedJacobianObserver = makeScopedIdentifiedElement( ddir->jacobianObservers, ddircb );
    auto scopedErrorObserver = makeScopedIdentifiedElement( solver.iterationPerformer()->errorEstimator()->errorObservers, errcb );
    auto scopedLineSearchObserver = makeScopedIdentifiedElement( solver.iterationPerformer()->newtonLinearSearch()->lineSearchObservers, lscb );

    // Run solver with zero initial guess
    MyVec x0( eqnRhs->inputSize() );
    eqnRhsScaled->normalize( x0 );
    solver.setInitialGuess( x0 );
    auto scopedIterationObserver = makeScopedIdentifiedElement( solver.iterationObservers, cb );
    auto status = solver.run();
    cout << solver.statusText( status ) << endl;
    }

// Uncomment to call the test at startup
// struct _{_() { testAlgebraicEqnSolver(); } }__;

} // anonymous namespace

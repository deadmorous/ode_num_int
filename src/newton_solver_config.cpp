// newton_solver_config.cpp

#include "newton_solver_config.h"
#include "foreachindex.h"
#include "ode_cvt.h"

using namespace std;

namespace ctm {

using namespace math;
using namespace mech;

namespace tools {
namespace cmd {
namespace numint {

NewtonSolverConfiguration::NewtonSolverConfiguration() :
    ddir( "simple" ),
    tstep( "euler" ),
    linsearch( "simple" ),
    errest( "simple" )
    {
    }

NewtonSolverComponents NewtonSolverConfiguration::apply(
        NewtonSolver<VDSimDyn>& solver,
        mech::ISimulationContextCtl *sc,
        const set< unsigned int >& frozenVarIds,
        double odeInitTime,
        const VectorTemplate< VDSimDyn >& odeInitState,
        const VectorTemplate< VDSimDyn >& odeInitGuess,
        bool normalized )
    {
    auto odeRhs = make_shared< SimDynOde >( sc );
    auto timeStepper = tstep.makeComponent();
    timeStepper->setOdeRhs( odeRhs );
    shared_ptr< VectorMapping<VDSimDyn> > eqnRhs = timeStepper;

    // Specify initial state; start transforming initial guess
    timeStepper->setInitialState( odeInitTime, odeInitState );
    auto x0 = timeStepper->computeEqnState( odeInitGuess );

    auto narrow = !frozenVarIds.empty();
    shared_ptr< VectorNarrowingMapping<VDSimDyn> > narrowedEqn;
    vector<unsigned int> disabledVars;
    if( narrow ) {
        narrowedEqn = make_shared< VectorNarrowingMapping<VDSimDyn> >();
        foreachIndex( eqnRhs->inputIds(), [&]( unsigned int id, unsigned int index ) {
            if( frozenVarIds.count( id ) > 0 )
                disabledVars.push_back( index );
            } );
        narrowedEqn->setMapping( eqnRhs );
        eqnRhs = narrowedEqn;
        narrowedEqn->setInputNarrowing( disabledVars, vec<double>( timeStepper->inputSize() ) );
        narrowedEqn->setOutputNarrowing( disabledVars );
        x0 = narrowedEqn->narrowInput( x0 );
        }

    auto reorder = true;
    shared_ptr< VectorReorderingMapping<VDSimDyn> > reorderedEqn;
    if( reorder ) {
        reorderedEqn = make_shared<CvtReorderingMapping>();
        reorderedEqn->setMapping( eqnRhs );
        eqnRhs = reorderedEqn;
        x0 = reorderedEqn->orderInput( x0 );
        }

    shared_ptr< MappingNormalizer<VDSimDyn> > eqnRhsNormalized;
    if( normalized ) {
        eqnRhsNormalized = make_shared< MappingNormalizer<VDSimDyn> >();
        eqnRhsNormalized->setMapping( eqnRhs );
        eqnRhs = eqnRhsNormalized;
        eqnRhsNormalized->normalize( x0 );
        }

    solver.setComponent( eqnRhs );
    solver.setComponent( ddir.makeComponent() );
    solver.setComponent( jtrim.makeComponent() );
    solver.setComponent( linsearch.makeComponent() );
    solver.setComponent( errest.makeComponent() );
    solver.setComponent( regularizer.makeComponent() );

    if( normalized )
        solver.setInitialGuess( eqnRhsNormalized->diagonalMapping()->VectorBijectiveMapping<VDSimDyn>::unmap( x0 ) );
    else
        solver.setInitialGuess( x0 );

    return NewtonSolverComponents(
                eqnRhs, odeRhs, timeStepper,
                narrowedEqn, narrow,
                reorderedEqn, reorder,
                disabledVars,
                eqnRhsNormalized, normalized );
    }

} // end namespace numint
} // end namespace cmd
} // end namespace tools
} // end namespace ctm

// OdeSolverConfiguration.h

#ifndef _ODESOLVER_ODESOLVERCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolver.h"
#include "OdeInitState.h"
#include "../ComponentConfiguration.h"
#include "../alg_solver/MappingNormalizer.h"
#include "../VectorNarrowingMapping.h"
#include "../VectorReorderingMapping.h"

namespace ctm {
namespace math {

template< class VD >
class OdeSolverComponents
    {
    public:
        OdeSolverComponents(
            const std::shared_ptr< math::OdeSolver<VD> >& solver,
            const std::shared_ptr< math::OdeSolver<VD> >& refSolver,
            const std::shared_ptr< math::OdeRhs<VD> >& odeRhs,
            const std::shared_ptr< math::VectorNarrowingMapping<VD> >& narrowedEqn,
            bool narrow,
            const std::shared_ptr< math::VectorReorderingMapping<VD> >& reorderedEqn,
            bool reordered,
            const std::vector<unsigned int>& disabledVars,
            const std::shared_ptr< math::MappingNormalizer<VD> >& eqnRhsNormalized,
            bool normalized ) :
                m_solver( solver ),
                m_refSolver( refSolver ),
                m_odeRhs( odeRhs ),
                m_narrowedEqn( narrowedEqn ),
                m_narrow( narrow ),
                m_reorderedEqn( reorderedEqn ),
                m_reordered( reordered ),
                m_disabledVars( disabledVars ),
                m_eqnRhsNormalized( eqnRhsNormalized ),
                m_normalized( normalized )
            {}

        std::shared_ptr< math::OdeSolver<VD> > solver() const {
            return m_solver;
            }

        std::shared_ptr< math::OdeSolver<VD> > refSolver() const {
            return m_refSolver;
            }

        std::shared_ptr< math::OdeRhs<VD> > odeRhs() const {
            return m_odeRhs;
            }

        std::shared_ptr< math::VectorNarrowingMapping<VD> > narrowedEqn() const {
            return m_narrowedEqn;
            }

        bool narrow() {
            return m_narrow;
            }

        std::shared_ptr< math::VectorReorderingMapping<VD> > reorderedEqn() const {
            return m_reorderedEqn;
            }

        bool reordered() const {
            return m_reordered;
            }

        std::vector<unsigned int> disabledVars() const {
            return m_disabledVars;
            }

        std::shared_ptr< math::MappingNormalizer<VD> > eqnRhsNormalized() const {
            return m_eqnRhsNormalized;
            }

        bool normalized() const {
            return m_normalized;
            }

    private:
        std::shared_ptr< math::OdeSolver<VD> > m_solver;
        std::shared_ptr< math::OdeSolver<VD> > m_refSolver;
        std::shared_ptr< math::OdeRhs<VD> > m_odeRhs;
        std::shared_ptr< math::VectorNarrowingMapping<VD> > m_narrowedEqn;
        bool m_narrow;
        std::shared_ptr< math::VectorReorderingMapping<VD> > m_reorderedEqn;
        bool m_reordered;
        std::vector<unsigned int> m_disabledVars;
        std::shared_ptr< math::MappingNormalizer<VD> > m_eqnRhsNormalized;
        bool m_normalized;
    };

template< class VD >
class OdeSolverOutputOption;

template< class VD >
class OdeSolverConfiguration : public ComponentConfiguration
    {
    public:
        class ParameterProvider : public OptionalParameters
            {
            public:
                typedef math::OdeSolver< VD > Solver;

                ParameterProvider() :
                    m_solver( OptionalParameters::Value("rk4") ),
                    m_odeSolverDuration( 0.001 ),
                    m_odeSolverOutputTiming( 0 )
                    {}

                Parameters parameters() const
                    {
                    Parameters result;
                    result["rhs"] = m_odeRhs;
                    result["solver"] = m_solver;
                    result["reference_solver"] = m_refSolver;
                    result["time"] = m_odeSolverDuration;
                    result["output_timing"] = m_odeSolverOutputTiming;
                    result["simout_basename"] = m_simoutCustomBaseName;
                    result["output_stats"] = m_statisticsOutput;
                    result["output_con"] = m_stepConsoleOutput;
                    result["output_file"] = m_stepFileOutput;
                    result["init_state"] = m_initState;
                    return result;
                    }

                void setParameters( const Parameters& parameters )
                    {
                    maybeLoadParameter( parameters, "rhs", m_odeRhs );
                    maybeLoadParameter( parameters, "solver", m_solver );
                    maybeLoadParameter( parameters, "reference_solver", m_refSolver );
                    maybeLoadParameter( parameters, "time", m_odeSolverDuration );
                    maybeLoadParameter( parameters, "output_timing", m_odeSolverOutputTiming );
                    maybeLoadParameter( parameters, "simout_basename", m_simoutCustomBaseName );
                    maybeLoadParameter( parameters, "output_stats", m_statisticsOutput );
                    maybeLoadParameter( parameters, "output_con", m_stepConsoleOutput );
                    maybeLoadParameter( parameters, "output_file", m_stepFileOutput );
                    maybeLoadParameter( parameters, "init_state", m_initState );
                    }

                Parameters helpOnParameters() const
                    {
                    Parameters result;
                    result["rhs"] = appendNestedHelp( m_odeRhs, "ODE right hand side" );
                    result["solver"] = appendNestedHelp( m_solver, "ODE solver to use" );
                    result["reference_solver"] = appendNestedHelp( m_refSolver, "ODE solver to use in order to obtain reference solution" );
                    result["time"] = "value specifies the duration of time integration period.";
                    result["output_timing"] =
                            "value specifies the time interval between successive\n"
                            "writes to the simout file (0 means each time step is written).";
                    result["simout_basename"] =
                            "Custom name for the simout file,\n"
                            "without the .simout suffix. Defaults to '_ode'.";
                    result["output_stats"] = appendNestedHelp( m_statisticsOutput, "Timing statistics output" );
                    result["output_con"] = appendNestedHelp( m_stepConsoleOutput, "Per-step console output" );
                    result["output_file"] = appendNestedHelp( m_stepFileOutput, "Per-step file output" );
                    result["init_state"] = appendNestedHelp( m_initState, "Initial state specification (if null, default state is set)" );
                    return result;
                    }

                std::shared_ptr< math::OdeRhs<VD> > odeRhs() const {
                    return m_odeRhs;
                    }

                std::shared_ptr< Solver > solver() const {
                    return m_solver;
                    }

                std::shared_ptr< Solver > refSolver() const {
                    return m_refSolver;
                    }

                double odeSolverDuration() const {
                    return m_odeSolverDuration;
                    }

                double odeSolverOutputTiming() const {
                    return m_odeSolverOutputTiming;
                    }

                std::string simoutCustomBaseName() const {
                    return m_simoutCustomBaseName;
                    }

                std::shared_ptr< OdeSolverOutputOption<VD> > statisticsOutput() const {
                    return m_statisticsOutput;
                    }

                std::shared_ptr< OdeSolverOutputOption<VD> > stepConsoleOutput() const {
                    return m_stepConsoleOutput;
                    }

                std::shared_ptr< OdeSolverOutputOption<VD> > stepFileOutput() const {
                    return m_stepFileOutput;
                    }

                std::shared_ptr< math::OdeInitState<VD> > initState() const {
                    return m_initState;
                    }

            private:
                std::shared_ptr< math::OdeRhs<VD> > m_odeRhs;
                std::shared_ptr< Solver > m_solver;
                std::shared_ptr< Solver > m_refSolver;
                double m_odeSolverDuration;
                double m_odeSolverOutputTiming;
                std::string m_simoutCustomBaseName;
                std::shared_ptr< OdeSolverOutputOption<VD> > m_statisticsOutput;
                std::shared_ptr< OdeSolverOutputOption<VD> > m_stepConsoleOutput;
                std::shared_ptr< OdeSolverOutputOption<VD> > m_stepFileOutput;
                std::shared_ptr< math::OdeInitState<VD> > m_initState;
            };

        OdeSolverConfiguration() :
            ComponentConfiguration( [this](){
                if( !m_parameterProvider )
                    m_parameterProvider = std::make_shared<ParameterProvider>();
                return m_parameterProvider;
            } )
            {}

        OdeSolverComponents<VD> apply(
                const std::set< unsigned int >& frozenVarIds,
                double odeInitTime,
                const math::VectorTemplate< VD >& odeInitState,
                bool normalized = false )
            {
            using namespace math;
            auto pp = parameterProvider();
            auto s = pp->solver();
            if( !s )
                throw cxx::exception( "ODE solver is not specified" );
            auto rs = pp->refSolver();
            auto odeRhs = pp->odeRhs();

            // TODO auto jtrim = parameterProvider()->jtrim();

            auto configureSolver = [&]( const decltype(s)& solver ) {
                if( !solver )
                    return;
                solver->setOdeRhs( odeRhs );

                // TODO: frozen vars, reorder, normalize

                // TODO: Jacobian trimmer

                auto getInitState = [pp]() -> std::shared_ptr< math::OdeInitState<VD> > {
                        auto initState = pp->initState();
                        if( initState   &&   dynamic_cast< TypeIdGetter< OdeInitState<VD> >* >(initState.get())->typeId() == "sim_dyn_initial" )
                            initState.reset();
                        return initState;
                    };

                if( auto initState = getInitState() ) {
                    auto state = initState->initialState();
                    auto requiredStateSize = solver->odeRhs()->varCount();
                    if( state.size() != requiredStateSize )
                        throw cxx::exception(
                                std::string("Invalid initial state size: expected ") +
                                cxx::FormatInt(requiredStateSize) + ", passed " + cxx::FormatInt(state.size()) );
                    solver->setInitialState( initState->initialTime(), state );
                    }
                else
                    solver->setInitialState( odeInitTime, odeInitState );
                };

            configureSolver( s );
            configureSolver( rs );

            return OdeSolverComponents<VD>(
                        s,
                        rs,
                        odeRhs,
                        std::shared_ptr< VectorNarrowingMapping<VD> >() /*narrowedEqn*/,
                        false /*narrow*/,
                        std::shared_ptr< VectorReorderingMapping<VD> >() /*reorderedEqn*/,
                        false /*reordered*/,
                        std::vector<unsigned int>() /*disabledVars*/,
                        std::shared_ptr< MappingNormalizer<VD> >() /*eqnRhsNormalized*/,
                        false /*normalized*/);
            }

        const ParameterProvider *parameterProvider() const
            {
            ComponentConfiguration::parameterProvider(); // Make sure to create an instance in m_parameterProvider
            return m_parameterProvider.get();
            }

    private:
        std::shared_ptr< ParameterProvider > m_parameterProvider;
    };

template< class VD >
class OdeSolverOutputOption :
    public Factory< OdeSolverOutputOption<VD> >,
    public OptionalParameters
    {
    public:
        virtual void beforeFirstStep(
            const OdeSolverConfiguration<VD> *solverConfig,
            const OdeSolverComponents<VD> *solverComponents) = 0;
        virtual void afterSolve() = 0;
    };

template< class VD >
inline std::vector< OdeSolverOutputOption<VD>* > initOdeOutput(
        const OdeSolverConfiguration<VD> *config,
        const OdeSolverComponents<VD> *components )
    {
    auto paramProvider = config->parameterProvider();
    std::vector< OdeSolverOutputOption<VD>* > outputOptions;
    auto collectOutputOption = [&]( const std::shared_ptr< OdeSolverOutputOption<VD> >& oo ) {
        if( oo ) {
            outputOptions.push_back( oo.get() );
            oo->beforeFirstStep( config, components );
            }
        };
    collectOutputOption( paramProvider->statisticsOutput() );
    collectOutputOption( paramProvider->stepConsoleOutput() );
    collectOutputOption( paramProvider->stepFileOutput() );
    return outputOptions;
    }

template< class VD >
inline void finalizeOdeOutput(
        const std::vector< OdeSolverOutputOption<VD>* >& outputOptions )
    {
    for( auto oo : outputOptions )
        oo->afterSolve();
    }

template< class VD >
inline void solveOde(
        const OdeSolverConfiguration<VD> *config,
        const OdeSolverComponents<VD> *components )
    {
    auto paramProvider = config->parameterProvider();
    auto solver = components->solver();

    if( !solver->odeRhs() )
        throw cxx::exception( "ODE right hand size is not specified" );

    auto outputOptions = initOdeOutput( config, components );

    auto startTime = solver->initialTime();
    auto endTime = startTime + paramProvider->odeSolverDuration();
    auto currentTime = startTime;
    do {
        solver->doStep();
        currentTime = solver->initialTime();
        }
    while( currentTime < endTime );
    finalizeOdeOutput( outputOptions );
    }

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODESOLVERCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

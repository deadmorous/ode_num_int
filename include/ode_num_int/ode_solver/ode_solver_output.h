// ode_solver_output.h

#ifndef _ODESOLVER_ODE_SOLVER_OUTPUT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODE_SOLVER_OUTPUT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "OdeSolverConfiguration.h"
#include "../timing/TimerCalibrator.h"
#include "OdeVarNameGenerator.h"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace ctm {
namespace math {

struct ObserverCallbackHelper
    {
    template< class Elements, class D, class... Args >
    cxx::ScopedIdentifiedElement< std::function<void(Args...)> > makeCb( Elements& elements, void (D::*method)(Args...) )
        {
        auto thisD = static_cast<D*>( this );
        return cxx::ScopedIdentifiedElement< std::function<void(Args...)> >( elements, [thisD, method](Args ... args) {
            ( thisD->*method )( args... );
            } );
        }

    template< class Elements, class D, class R, class... Args >
    cxx::ScopedIdentifiedElement< std::function<R(Args...)> > makeCb( Elements& elements, R (D::*method)(Args...) )
        {
        auto thisD = static_cast<D*>( this );
        return cxx::ScopedIdentifiedElement< std::function<R(Args...)> >( elements, [thisD, method](Args ... args) -> R {
            return ( thisD->*method )( args... );
            } );
        }
    };

template< class VD >
class OdeSolverStreamOutputOption :
    public OdeSolverOutputOption<VD>
    {
    public:
        typedef math::VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        Parameters parameters() const
            {
            Parameters result;
            result["file_name"] = m_fileName;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "file_name", m_fileName );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["file_name"] = "Name of file to write the report to; if empty, stdout is used";
            return result;
            }

        std::string fileName() const {
            return m_fileName;
            }

    private:
        std::string m_fileName;

    protected:
        class D
            {
            public:
                D( const OdeSolverConfiguration<VD> *solverConfig, const OdeSolverComponents<VD> *solverComponents,
                            const std::string& fileName ) :
                    m_outputTiming( solverConfig->parameterProvider()->odeSolverOutputTiming() ),
                    m_startTime( solverComponents->solver()->initialTime() ),
                    m_reportTime( m_startTime - 2*m_outputTiming ),
                    m_outputFile( fileName.empty()? nullptr: new std::ofstream( fileName ) ),
                    m_os( fileName.empty()? std::cout :   *m_outputFile )
                    {
                    if( !fileName.empty() ) {
                        if( m_outputFile->fail() )
                            throw cxx::exception( std::string("Failed to open output file '") + fileName + "'" );
                        m_os << std::setprecision(16);
                        }
                    }

                std::ostream& os() {
                    return m_os;
                    }

                bool outputNow( real_type currentTime, bool force = false )
                    {
                    bool result = force   ||   m_outputTiming <= 0   ||
                        ( currentTime - m_startTime ) / m_outputTiming >=
                        ( m_reportTime - m_startTime ) / m_outputTiming + 1;
                    if ( result )
                        m_reportTime = currentTime;
                    return result;
                    }
            private:
                real_type m_outputTiming;
                real_type m_startTime;
                real_type m_reportTime;
                std::unique_ptr<std::ofstream> m_outputFile;
                std::ostream& m_os;
            };
    };

template< class VD >
class OdeSolverStatisticsOutput :
    public OdeSolverStreamOutputOption<VD>,
    public FactoryMixin< OdeSolverStatisticsOutput<VD>, OdeSolverOutputOption<VD> >
    {
    public:
        typedef math::VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        void beforeFirstStep(
            const OdeSolverConfiguration<VD> *solverConfig,
            const OdeSolverComponents<VD> *solverComponents )
            {
            // TODO: use std::make_unique when moving to c++14
            m_d = std::unique_ptr<D>( new D(
                solverConfig, solverComponents, this->fileName() ) );
            }

        void afterSolve()
            {
            m_d->printStats();
            m_d.reset();
            }

    private:
        struct D :
                ObserverCallbackHelper,
                OdeSolverStreamOutputOption<VD>::D
            {
            const OdeSolverConfiguration<VD> *m_solverConfig;
            const OdeSolverComponents<VD> *m_solverComponents;
            real_type startTime;

            unsigned int nSteps = 0;
            unsigned int nRejectedSteps = 0;
            unsigned int nStepChanges = 0;
            unsigned int nTruncatedSteps = 0;
            real_type stepSizeSum = 0;

            cxx::ScopedIdentifiedElement< typename math::OdeSolverPostObservers<VD>::cb_type > m_cbAfterStep;
            cxx::ScopedIdentifiedElement< typename math::OdeRhsPreObservers<VD>::cb_type > m_cbBeforeOdeRhs;
            cxx::ScopedIdentifiedElement< typename math::OdeRhsPostObservers<VD>::cb_type > m_cbAfterOdeRhs;
            cxx::ScopedIdentifiedElement< typename math::JacobianRefreshObservers::cb_type > m_cbJRefresh;

            sys::TimingStats odeRhsTimingStats;
            OpaqueTickCounter odeRhsTimer;
            unsigned int odeRhsInvocationCount4J = 0;
            bool computingJacobian = false;
            sys::TimingStats jacobianTimingStats;
            OpaqueTickCounter computeJacobianTimer;

            sys::TimerCalibrator tc;
            OpaqueTickCounter solverTotalTimer;


            D( const OdeSolverConfiguration<VD> *solverConfig, const OdeSolverComponents<VD> *solverComponents, const std::string& fileName ) :
                OdeSolverStreamOutputOption<VD>::D( solverConfig, solverComponents, fileName ),
                m_solverConfig( solverConfig ),
                m_solverComponents( solverComponents ),
                startTime( solverComponents->solver()->initialTime() ),
                m_cbAfterStep( makeCb( solverComponents->solver()->odeSolverPostObservers, &D::afterStep ) ),
                m_cbBeforeOdeRhs( makeCb( solverComponents->solver()->odeRhs()->odeRhsPreObservers, &D::beforeOdeRhs ) ),
                m_cbAfterOdeRhs( makeCb( solverComponents->solver()->odeRhs()->odeRhsPostObservers, &D::afterOdeRhs ) ),
                m_cbJRefresh( makeCb( solverComponents->solver()->jacobianRefreshObservers, &D::onStartStopJRefresh ) ),
                solverTotalTimer( true )
                {}

            void afterStep( real_type h,
                             bool stepAccepted,
                             bool stepSizeChanged,
                             bool stepTruncated,
                             real_type errorNorm,
                             unsigned int izfTrunc,
                             int transitionType,
                             const math::OdeSolver<VD>* s )
                {
                ++nSteps;
                if( !stepAccepted )
                    ++nRejectedSteps;
                if( stepSizeChanged )
                    ++nStepChanges;
                if( stepTruncated )
                    ++nTruncatedSteps;
                if( stepAccepted )
                    stepSizeSum += h;
                }

            void beforeOdeRhs( real_type, const V&, const math::OdeRhs<VD>* ) {
                odeRhsTimer.Start();
                }

            void afterOdeRhs( real_type, const V&, const V&, const math::OdeRhs<VD>* )
                {
                odeRhsTimingStats.add( odeRhsTimer.Stop() );
                if( computingJacobian )
                    ++odeRhsInvocationCount4J;
                }

            void onStartStopJRefresh( bool start )
                {
                computingJacobian = start;
                if( start )
                    computeJacobianTimer.Start();
                else
                    jacobianTimingStats.add( computeJacobianTimer.Stop() );
                }

            void printStats()
                {
                using namespace std;
                tc.stop();
                auto solverTotalTime = solverTotalTimer.Stop();
                auto solver = m_solverComponents->solver();

                auto& os = this->os();
                auto reportTimingStats = [&]( const char *msg, const sys::TimingStats& st ) {
                    if( st.count() == 0 )
                        return;
                    os << msg
                         << ": total time = " << tc.formatTimeInterval( st.time() )
                         << ", avg. time = " << tc.formatTimeInterval( st.averageTime() )
                         << ", count = " << st.count() << endl;
                    };

                os << "Total time: " << tc.formatTimeInterval( solverTotalTime ) << endl;
                if( tc.toMsecAvailable() )
                    os << "Speed (real time / CPU time): " << ( solver->initialTime() - startTime ) * 1000 / tc.toMsec( solverTotalTime ) << endl;
                reportTimingStats( "TIMING: Jacobian", jacobianTimingStats );
                reportTimingStats( "TIMING: ODE RHS", odeRhsTimingStats );
                auto luTimingStats = solver->luTimingStats;
                reportTimingStats( "TIMING: LU factorize", luTimingStats.factorizeTiming );
                reportTimingStats( "TIMING: LU solve", luTimingStats.solveTiming );
                reportTimingStats( "TIMING: LU set mx", luTimingStats.setMatrixTiming );
                reportTimingStats( "TIMING: LU set mx fast", luTimingStats.setMatrixFastTiming );
                reportTimingStats( "TIMING: LU update", luTimingStats.updateTiming );

                reportTimingStats( "TIMING: DBG 0", solver->tstat0 );
                reportTimingStats( "TIMING: DBG 1", solver->tstat1 );
                reportTimingStats( "TIMING: DBG 2", solver->tstat2 );
                reportTimingStats( "TIMING: DBG 3", solver->tstat3 );
                reportTimingStats( "TIMING: DBG 4", solver->tstat4 );
                reportTimingStats( "TIMING: DBG 5", solver->tstat5 );
                reportTimingStats( "TIMING: DBG 6", solver->tstat6 );
                reportTimingStats( "TIMING: DBG 7", solver->tstat7 );
                reportTimingStats( "TIMING: DBG 8", solver->tstat8 );
                reportTimingStats( "TIMING: DBG 9", solver->tstat9 );

                os << "Number of ODE RHS evaluations to refresh Jacobian: " << odeRhsInvocationCount4J << endl;
                os << "Total number of ODE RHS evaluations: " << odeRhsTimingStats.count()
                     << " = " << (odeRhsTimingStats.count()-odeRhsInvocationCount4J)
                     << " + " << odeRhsInvocationCount4J << endl
                     << "Average step size: " << stepSizeSum / ( nSteps - nRejectedSteps ) << endl
                     << "Total steps: " << nSteps << endl
                     << "Rejected steps: " << nRejectedSteps << " (" << 100.*nRejectedSteps/nSteps << "%)" << endl
                     << "Step changes: " << nStepChanges << " (" << 100.*nStepChanges/nSteps << "%)" << endl
                     << "Truncated steps: " << nTruncatedSteps << " (" << 100.*nTruncatedSteps/nSteps << "%)" << endl;
                }

            };
        std::unique_ptr< D > m_d;
    };

template< class VD >
class OdeSolverStepGeneralConsoleOutput :
    public OdeSolverStreamOutputOption<VD>,
    public FactoryMixin< OdeSolverStepGeneralConsoleOutput<VD>, OdeSolverOutputOption<VD> >
    {
    public:
        typedef math::VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        void beforeFirstStep(
            const OdeSolverConfiguration<VD> *solverConfig,
            const OdeSolverComponents<VD> *solverComponents)
            {
            // TODO: use std::make_unique when moving to c++14
            m_d = std::unique_ptr<D>(
                new D( solverConfig, solverComponents, this->fileName() ) );
            }

        void afterSolve()
            {
            m_d.reset();
            }

    private:
        struct D :
                ObserverCallbackHelper,
                OdeSolverStreamOutputOption<VD>::D
            {
            unsigned int nSteps = 0;

            cxx::ScopedIdentifiedElement< typename math::OdeSolverPostObservers<VD>::cb_type > m_cbAfterStep;
            cxx::ScopedIdentifiedElement< typename math::NewtonSolverIterationObservers<VD>::cb_type > m_cbAfterIteration;

            D( const OdeSolverConfiguration<VD> *solverConfig, const OdeSolverComponents<VD> *solverComponents, const std::string& fileName ) :
                OdeSolverStreamOutputOption<VD>::D( solverConfig, solverComponents, fileName ),
                m_cbAfterStep( makeCb( solverComponents->solver()->odeSolverPostObservers, &D::afterStep ) ),
                m_cbAfterIteration( makeCb( solverComponents->solver()->iterationObservers, &D::afterIteration ) )
                {}

            void afterStep( real_type h,
                            bool stepAccepted,
                            bool stepSizeChanged,
                            bool stepTruncated,
                            real_type errorNorm,
                            unsigned int izfTrunc,
                            int transitionType,
                            const math::OdeSolver<VD>* s )
                {
                using namespace std;
                ++nSteps;

                auto currentTime = s->initialTime();
                if( !this->outputNow( currentTime, stepTruncated ) )
                    return;

                auto& os = this->os();
                os << "step " << nSteps
                     << ", time = " << s->initialTime()
                     << ", h = " << h
                     << ", err = " << errorNorm
                     << ", " << (stepAccepted? "accepted": "rejected");
                if( stepSizeChanged )
                    os << ", step size changed";
                if( stepTruncated ) {
                    os << ", step truncated";
                    if( izfTrunc != ~0u )
                        os << " due to " << s->odeRhs()->describeZeroFunction( izfTrunc ) << " " << (transitionType>0? '+': transitionType<0? '-': '?');
                    }
                os << endl;
                }

            bool afterIteration( unsigned int iterationNumber,
                                 const math::NewtonSolverInterface<VD>* solver )
                {
                using namespace std;

                auto itperf = solver->iterationPerformer();
                auto alpha = itperf->currentStepRatio();
                auto statusMsg = itperf->errorEstimator()->currentStatusMessage();

                auto& os = this->os();

                os << "  Newton it. " << ( iterationNumber + 1 )
                     << ", alpha=" << alpha
                     << ", " << statusMsg
                     << endl;

                return false;
                }
            };
        std::unique_ptr< D > m_d;
    };

template< class VD >
class OdeSolverStepSolutionConsoleOutput :
    public OdeSolverStreamOutputOption<VD>,
    public FactoryMixin< OdeSolverStepSolutionConsoleOutput<VD>, OdeSolverOutputOption<VD> >
    {
    public:
        typedef math::VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverStepSolutionConsoleOutput() : m_sizeHalfLimit( 5 ) {}

        void beforeFirstStep(
            const OdeSolverConfiguration<VD> *solverConfig,
            const OdeSolverComponents<VD> *solverComponents)
            {
            // TODO: use std::make_unique when moving to c++14
            m_d = std::unique_ptr<D>( new D(
                solverConfig, solverComponents, m_sizeHalfLimit, this->fileName() ) );
            auto s = solverComponents->solver().get();
            m_d->writeHeaders( s );
            m_d->write( s );
            }

        void afterSolve()
            {
            m_d.reset();
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolverStreamOutputOption<VD>::parameters();
            result["size_half_lim"] = m_sizeHalfLimit;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolverStreamOutputOption<VD>::setParameters( parameters );
            this->maybeLoadParameter( parameters, "size_half_lim", m_sizeHalfLimit );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolverStreamOutputOption<VD>::helpOnParameters();
            result["size_half_lim"] = "Maximum number of elements to output in the beginning and end of the state vector";
            return result;
            }

    private:
        unsigned int m_sizeHalfLimit;

        struct D :
                ObserverCallbackHelper,
                OdeSolverStreamOutputOption<VD>::D
            {
            std::vector< unsigned int > m_indices;

            cxx::ScopedIdentifiedElement< typename math::OdeSolverPostObservers<VD>::cb_type > m_cbAfterStep;

            D( const OdeSolverConfiguration<VD> *solverConfig, const OdeSolverComponents<VD> *solverComponents,
               unsigned int sizeHalfLimit, const std::string& fileName ) :
                OdeSolverStreamOutputOption<VD>::D( solverConfig, solverComponents, fileName ),
                m_cbAfterStep( makeCb( solverComponents->solver()->odeSolverPostObservers, &D::afterStep ) )
                {
                auto rhs = solverComponents->solver()->odeRhs();
                auto n = rhs->varCount();
                auto n2 = rhs->secondOrderVarCount();

                auto addIndices = [this, sizeHalfLimit]( unsigned int ibegin, unsigned int iend ) {
                    for( unsigned int i=0, n=iend-ibegin; i<n; ) {
                        if( i == sizeHalfLimit && n > 2*sizeHalfLimit ) {
                            i = n - sizeHalfLimit;
                            continue;
                            }
                        m_indices.push_back( ibegin + i );
                        ++i;
                        }
                    };

                if( n2 == 0 )
                    addIndices( 0, n );
                else {
                    addIndices( 0, n2 );
                    addIndices( n2, 2*n2 );
                    addIndices( 2*n2, n );
                    }
                }

            void writeHeaders( const math::OdeSolver<VD>* s )
                {
                using namespace std;
                auto& os = this->os();
                os << "time";
                auto n2 = s->odeRhs()->secondOrderVarCount();
                for( auto i : m_indices ) {
                    os << "\t";
                    string name;
                    if( n2 == 0   ||   i<n2 )
                        name = string("x") + cxx::FormatInt(i);
                    else if( i<2*n2 )
                        name = string("v") + cxx::FormatInt(i-n2);
                    else
                        name = string("z") + cxx::FormatInt(i-2*n2);
                    os << name;
                    }
                os << endl;
                }

            void write( const math::OdeSolver<VD>* s )
                {
                using namespace std;
                auto& os = this->os();

                auto x = s->initialState();
                os << s->initialTime();
                for( auto i : m_indices )
                    os << "\t" << x[i];
                os << endl;
                }

            void afterStep( real_type h,
                            bool stepAccepted,
                            bool stepSizeChanged,
                            bool stepTruncated,
                            real_type errorNorm,
                            unsigned int izfTrunc,
                            int transitionType,
                            const math::OdeSolver<VD>* s )
                {
                auto currentTime = s->initialTime();
                if( stepAccepted   &&   this->outputNow( currentTime ) )
                    write( s );
                }
            };
        std::unique_ptr< D > m_d;
    };

template< class VD >
class OdeSolverStepSolutionColumnwiseOutput :
    public OdeSolverStreamOutputOption<VD>,
    public FactoryMixin< OdeSolverStepSolutionColumnwiseOutput<VD>, OdeSolverOutputOption<VD> >
    {
    public:
        typedef math::VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        OdeSolverStepSolutionColumnwiseOutput() : m_maxSteps( 10 ) {}

        void beforeFirstStep(
            const OdeSolverConfiguration<VD> *solverConfig,
            const OdeSolverComponents<VD> *solverComponents )
            {
            // TODO: use std::make_unique when moving to c++14
            m_d = std::unique_ptr<D>( new D(
                solverConfig, solverComponents, m_maxSteps, this->fileName() ) );
            m_d->accum();
            }

        void afterSolve()
            {
            m_d->write();
            m_d.reset();
            }

        Parameters parameters() const
            {
            Parameters result = OdeSolverStreamOutputOption<VD>::parameters();
            result["max_steps"] = m_maxSteps;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            OdeSolverStreamOutputOption<VD>::setParameters( parameters );
            this->maybeLoadParameter( parameters, "max_steps", m_maxSteps );
            }

        Parameters helpOnParameters() const
            {
            Parameters result = OdeSolverStreamOutputOption<VD>::helpOnParameters();
            result["max_steps"] = "Maximum number of steps to output to the columns of the output file";
            return result;
            }

    private:
        unsigned int m_maxSteps;

        struct D :
                ObserverCallbackHelper,
                OdeSolverStreamOutputOption<VD>::D
            {
            unsigned int m_maxSteps;
            std::vector< std::pair<real_type, V> > m_columns;
            std::shared_ptr< math::OdeSolver<VD> > m_solver;

            cxx::ScopedIdentifiedElement< typename math::OdeSolverPostObservers<VD>::cb_type > m_cbAfterStep;

            D( const OdeSolverConfiguration<VD> *solverConfig, const OdeSolverComponents<VD> *solverComponents,
               unsigned int maxSteps, const std::string& fileName ) :
                OdeSolverStreamOutputOption<VD>::D( solverConfig, solverComponents, fileName ),
                m_maxSteps( maxSteps ),
                m_solver( solverComponents->solver() ),
                m_cbAfterStep( makeCb( solverComponents->solver()->odeSolverPostObservers, &D::afterStep ) )
                {}

            void afterStep( real_type h,
                            bool stepAccepted,
                            bool stepSizeChanged,
                            bool stepTruncated,
                            real_type errorNorm,
                            unsigned int izfTrunc,
                            int transitionType,
                            const math::OdeSolver<VD>* s )
                {
                if( stepAccepted )
                    accum();
                }

            void accum()
                {
                if( m_columns.size() >= m_maxSteps )
                    return;
                auto currentTime = m_solver->initialTime();
                if( !this->outputNow( currentTime ) )
                    return;
                m_columns.push_back( std::make_pair( currentTime, m_solver->initialState() ) );
                }

            void write()
                {
                math::OdeVarNameGenerator<VD> nameGen( *m_solver->odeRhs() );
                auto& os = this->os();
                os << "index\tname";
                auto ncols = m_columns.size();
                for( decltype(ncols) icol=0; icol<ncols; ++icol )
                    os << "\tn" << icol;
                os << std::endl;
                os << "-1\ttime";
                for( decltype(ncols) icol=0; icol<ncols; ++icol )
                    os << "\t" << m_columns[icol].first;
                os << std::endl;
                auto nvars = m_solver->odeRhs()->varCount();
                for( decltype(nvars) ivar=0; ivar<nvars; ++ivar ) {
                    os << ivar << "\t" << nameGen.varNameByIndex( ivar );
                    for( decltype(ncols) icol=0; icol<ncols; ++icol )
                        os << "\t" << m_columns[icol].second[ivar];
                    os << std::endl;
                    }
                }
            };
        std::unique_ptr< D > m_d;
    };

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODE_SOLVER_OUTPUT_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// ode_solver.h

#ifndef _ODE_SOLVER_H_
#define _ODE_SOLVER_H_

#include "ode_time_stepper.h"
#include "ctmstd/m_tolerance.h"
#include "lu/util.h"
#include "algebraic_eqn_solver.h" // JacobianTrimmer, TODO better
#include <algorithm>
#include <list>

namespace ctm {
namespace math {

template< class VD >
using ErrorNormCalculatorObservers = cxx::Observers<
    const VectorTemplate< VD >& /*error*/,
    const typename VD::value_type /*errorNorm*/ >;

template< class VD >
class ErrorNormCalculator :
    public Factory< ErrorNormCalculator<VD> >,
    public OdeRhsHolder<VD>,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        ErrorNormCalculatorObservers<VD> errorNormCalculatorObservers;

        virtual real_type errorNorm( const V& error ) const = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        ErrorNormCalculatorHolder, ErrorNormCalculator,
        errorNormCalculator, setErrorNormCalculator,
        onErrorNormCalculatorChanged, offErrorNormCalculatorChanged )



template< class VD >
class ErrorInfNormCalculator :
    public ErrorNormCalculator<VD>,
    public FactoryMixin< ErrorInfNormCalculator<VD>, ErrorNormCalculator<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        real_type errorNorm( const V& error ) const
            {
            auto errorNorm = error.infNorm();
            this->errorNormCalculatorObservers( error, errorNorm );
            return errorNorm;
            }
    };



template< class VD >
using OdeStepSizeControllerObservers = cxx::Observers<
    const typename VD::value_type /*oldStepSize*/,
    const typename VD::value_type /*newStepSize*/,
    bool /*stepAccepted*/,
    bool /*stepSizeChanged*/ >;

template< class VD >
class OdeStepSizeController :
    public Factory< OdeStepSizeController<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        struct Result
            {
            real_type newStepSize;
            bool acceptStep;
            bool changeStepSize;
            Result();
            Result(
                real_type newStepSize,
                bool acceptStep,
                bool changeStepSize ) :
                newStepSize( newStepSize ),
                acceptStep( acceptStep ),
                changeStepSize( changeStepSize )
                {}
            };

        OdeStepSizeControllerObservers<VD> odeStepSizeControllerObservers;

        virtual Result controlStepSize(
            real_type currentStepSize,
            real_type errorNorm,
            real_type methodOrder ) const = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        OdeStepSizeControllerHolder, OdeStepSizeController,
        odeStepSizeController, setOdeStepSizeController,
        onOdeStepSizeControllerChanged, offOdeStepSizeControllerChanged )



template< class VD >
class OdeEventController :
    public OdeRhsHolder<VD>,
    public Factory< OdeEventController<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef std::function<void(V&, real_type)> Interpolator;

        virtual void reset() = 0;
        virtual void atStepStart( const V& x1, real_type t1 ) = 0;
        virtual bool atStepEnd(
                const V& x1, V& x2, real_type& t2, Interpolator interpolate,
                unsigned int *izfTrunc = nullptr, int *transitionType = nullptr ) = 0;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        OdeEventControllerHolder, OdeEventController,
        eventController, setEventController,
        onEventControllerChanged, offEventControllerChanged)



template< class VD >
class OdeStepSizeSimpleController :
    public OdeStepSizeController<VD>,
    public FactoryMixin< OdeStepSizeSimpleController<VD>, OdeStepSizeController<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;
        typedef typename OdeStepSizeSimpleController<VD>::Result Result;
        typedef OptionalParameters::Parameters Parameters;

        OdeStepSizeSimpleController() :
            m_tolerance( 1e-6 ),
            m_insuranceCoefficient( 0.8 ),
            m_incMinFactor( 1.3 ),
            m_incMaxFactor( 2 ),
            m_decMaxFactor( 0.1 ),
            m_maxThreshold( 0 )
            {}

        Result controlStepSize(
            real_type currentStepSize,
            real_type errorNorm,
            real_type methodOrder ) const
            {
            if( errorNorm == 0 )
                return Result( currentStepSize, true, false );
            real_type p = std::pow( m_tolerance/errorNorm, 1./(methodOrder+1) );
            real_type pd = p * m_insuranceCoefficient;
            if( p >= 1 ) {
                if( m_maxThreshold > 0   &&   pd*currentStepSize > m_maxThreshold ) {
                    pd = m_maxThreshold / currentStepSize;
                    if( pd < 1 )
                        return Result( currentStepSize, true, false );
                    }
                if( m_incMinFactor > 0   &&   pd < m_incMinFactor )
                    return Result( currentStepSize, true, false );
                if( m_incMaxFactor > 0   &&   pd > m_incMaxFactor )
                    pd = m_incMaxFactor;
                return Result( currentStepSize*pd, true, true );
                }
            else if( p > 1 - GEOM_EPS )
                return Result( currentStepSize, true, false );
            else {
                if( m_decMaxFactor > 0   &&   pd < m_decMaxFactor )
                    pd = m_decMaxFactor;
                return Result( currentStepSize*pd, false, true );
                }
            }

        Parameters parameters() const
            {
            Parameters result;
            result["tolerance"] = m_tolerance;
            result["insurance"] = m_insuranceCoefficient;
            result["inc_min_factor"] = m_incMinFactor;
            result["inc_max_factor"] = m_incMaxFactor;
            result["dec_max_factor"] = m_decMaxFactor;
            result["max_threshold"] = m_maxThreshold;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "tolerance", m_tolerance );
            this->maybeLoadParameter( parameters, "insurance", m_insuranceCoefficient );
            this->maybeLoadParameter( parameters, "inc_min_factor", m_incMinFactor );
            this->maybeLoadParameter( parameters, "inc_max_factor", m_incMaxFactor );
            this->maybeLoadParameter( parameters, "dec_max_factor", m_decMaxFactor );
            this->maybeLoadParameter( parameters, "max_threshold", m_maxThreshold );
            }

    private:
        real_type m_tolerance;
        real_type m_insuranceCoefficient;
        real_type m_incMinFactor;
        real_type m_incMaxFactor;
        real_type m_decMaxFactor;
        real_type m_maxThreshold;
    };



template< class VD > class OdeSolver;

template< class VD >
using OdeSolverPreObservers = cxx::Observers<
    typename VD::value_type /*stepSize*/,
    const OdeSolver<VD>* >;

template< class VD >
using OdeSolverPostObservers = cxx::Observers<
    typename VD::value_type /*stepSize*/,
    bool /*stepAccepted*/,
    bool /*stepSizeChanged*/,
    bool /*stepTruncated*/,
    typename VD::value_type /*errorNorm*/,
    unsigned int /*izfTrunc*/,
    int /*transitionType*/,
    const OdeSolver<VD>* >;

using JacobianRefreshObservers = cxx::Observers< bool /*start*/ >;

template< class VD >
class OdeSolverComponent;

template< class VD >
class OdeSolver :
    public OdeRhsHolder<VD>,
    public Factory< OdeSolver<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        OdeSolverPreObservers<VD> odeSolverPreObservers;
        OdeSolverPostObservers<VD> odeSolverPostObservers;
        JacobianRefreshObservers jacobianRefreshObservers;
        NewtonSolverIterationObservers<VD> iterationObservers;
        typename LUFactorizer<real_type>::TimingStats luTimingStats;

        // deBUG, TODO: Remove
        sys::TimingStats tstat0;
        sys::TimingStats tstat1;
        sys::TimingStats tstat2;
        sys::TimingStats tstat3;
        sys::TimingStats tstat4;
        sys::TimingStats tstat5;
        sys::TimingStats tstat6;
        sys::TimingStats tstat7;
        sys::TimingStats tstat8;
        sys::TimingStats tstat9;

        OdeSolver() :
            m_initialStepSize( 0.01 ),
            m_stepSizeMinThreshold( 1e-16 )
            {}

        real_type initialStepSize() const {
            return m_initialStepSize;
            }

        virtual void setInitialStepSize( real_type initialStepSize ) {
            m_initialStepSize = initialStepSize;
            }

        real_type stepSizeMinThreshold() const {
            return m_stepSizeMinThreshold;
            }

        virtual void setStepSizeMinThreshold( real_type stepSizeMinThreshold ) {
            m_stepSizeMinThreshold = stepSizeMinThreshold;
            }

        virtual int order() const = 0;
        virtual real_type initialTime() const = 0;
        virtual V initialState() const = 0;
        virtual void setInitialState( real_type initialTime, const V& initialState, bool soft = false ) = 0;
        virtual void doStep() = 0;

        inline Parameters parameters() const;
        inline void setParameters( const Parameters & parameters );
        inline Parameters helpOnParameters() const;
        inline virtual std::map<std::string, sys::TimingStats> timingStats() const;

    private:
        real_type m_initialStepSize;
        real_type m_stepSizeMinThreshold;
        friend class OdeSolverComponent<VD>;
        std::list< OdeSolverComponent<VD>* > m_components;
    };

CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(
        OdeSolverHolder, OdeSolver,
        odeSolver, setOdeSolver,
        onOdeSolverChanged, offOdeSolverChanged)



template< class VD >
class OdeSolverComponent
    {
    public:
        explicit OdeSolverComponent( OdeSolver<VD>& solver ) : m_solver( solver ) {
            m_solver.m_components.push_back( this );
            m_iter = m_solver.m_components.end();
            --m_iter;
            }
        ~OdeSolverComponent() {
            m_solver.m_components.erase( m_iter );
            }

        virtual void saveParameters( OptionalParameters::Parameters& /*parameters*/ ) const {}
        virtual void loadParameters( const OptionalParameters::Parameters& /*parameters*/ ) {}
        virtual void addHelpOnParameters( OptionalParameters::Parameters& /*help*/ ) {}
        virtual std::map<std::string, sys::TimingStats> timingStats() const {
            return std::map<std::string, sys::TimingStats>();
        }

    protected:
        OdeSolver<VD>& solver() {
            return m_solver;
            }

    private:
        OdeSolver<VD>& m_solver;
        typename std::list< OdeSolverComponent<VD>* >::iterator m_iter;
    };



template< class VD >
inline auto OdeSolver<VD>::parameters() const -> Parameters
    {
    Parameters result;
    result["h_init"] = m_initialStepSize;
    result["h_min"] = m_stepSizeMinThreshold;
    for( auto component : m_components )
        component->saveParameters( result );
    return result;
    }

template< class VD >
inline void OdeSolver<VD>::setParameters( const Parameters& parameters )
    {
    maybeSetParameter( parameters, "h_init", std::bind( &OdeSolver<VD>::setInitialStepSize, this, std::placeholders::_1 ) );
    maybeSetParameter( parameters, "h_min", std::bind( &OdeSolver<VD>::setStepSizeMinThreshold, this, std::placeholders::_1 ) );
    for( auto component : m_components )
        component->loadParameters( parameters );
    }

template< class VD >
inline auto OdeSolver<VD>::helpOnParameters() const -> Parameters
    {
    Parameters result;
    result["h_init"] =
            "For methods without step size control, the step size;\n"
            "otherwise, the initial value of the step size.";
    result["h_min"] =
            "When the step size becomes less than this threshold, the\n"
            "integration stops. Notice that zero value means that the\n"
            "threshold is disabled.";
    for( auto component : m_components )
        component->addHelpOnParameters( result );
    return result;
    }

template< class VD >
inline std::map<std::string, sys::TimingStats> OdeSolver<VD>::timingStats() const
    {
    std::map<std::string, sys::TimingStats> result;
    for( auto component : m_components ) {
        auto cts = component->timingStats();
        for( const auto& item : cts ) {
            if( result.find( item.first ) != result.end() )
                throw cxx::exception( std::string("Timing statistics key is not unique: ") + item.first );
            result.insert( item );
            }
        }
    return result;
    }



template< class VD >
class OdeSolverErrorNormCalculator :
    public OdeSolverComponent<VD>,
    public ErrorNormCalculatorHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverErrorNormCalculator( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            auto updateOdeRhs = [this] {
                this->errorNormCalculator()->setOdeRhs( this->solver().odeRhs() );
                };
            this->onErrorNormCalculatorChanged( updateOdeRhs );
            solver.onOdeRhsChanged( updateOdeRhs );
            if( !typeId.empty() )
                this->setErrorNormCalculator( Factory< ErrorNormCalculator<VD> >::newInstance( typeId ) );
            }

        OdeSolverErrorNormCalculator( const OdeSolverErrorNormCalculator<VD>& ) = delete;
        OdeSolverErrorNormCalculator<VD>& operator=( const OdeSolverErrorNormCalculator<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["enorm"] = this->errorNormCalculator();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "enorm",
                        std::bind( &ErrorNormCalculatorHolder<VD>::setErrorNormCalculator, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["enorm"] = OptionalParameters::appendNestedHelp(
                        this->errorNormCalculator(),
                        "Error norm calculator" );
            }
    };



template< class VD >
class OdeSolverStepSizeController :
    public OdeSolverComponent<VD>,
    public OdeStepSizeControllerHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverStepSizeController( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setOdeStepSizeController( Factory< OdeStepSizeController<VD> >::newInstance( typeId ) );
            }

        OdeSolverStepSizeController( const OdeSolverStepSizeController<VD>& ) = delete;
        OdeSolverStepSizeController<VD>& operator=( const OdeSolverStepSizeController<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["stepsizectl"] = this->odeStepSizeController();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "stepsizectl",
                        std::bind( &OdeStepSizeControllerHolder<VD>::setOdeStepSizeController, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["stepsizectl"] = OptionalParameters::appendNestedHelp(
                        this->odeStepSizeController(),
                        "Step size control algorithm" );
            }
    };



template< class VD >
class OdeSolverEventController :
    public OdeSolverComponent<VD>,
    public OdeEventControllerHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverEventController( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setEventController( Factory< OdeEventController<VD> >::newInstance( typeId ) );

            auto setControllerRhs = [this]() {
                if( auto ec = this->eventController() )
                    ec->setOdeRhs( this->solver().odeRhs() );
                };
            this->onEventControllerChanged( setControllerRhs );
            solver.onOdeRhsChanged( setControllerRhs );
            }

        OdeSolverEventController( const OdeSolverEventController<VD>& ) = delete;
        OdeSolverEventController<VD>& operator=( const OdeSolverEventController<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["eventctl"] = this->eventController();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "eventctl",
                        std::bind( &OdeEventControllerHolder<VD>::setEventController, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["eventctl"] = OptionalParameters::appendNestedHelp(
                        this->eventController(),
                        "Event controller algorithm" );
            }
    };



template< class VD >
class OdeSolverJacobianTrimmer :
    public OdeSolverComponent<VD>,
    public JacobianTrimmerHolder<VD>
    {
    public:
        typedef OptionalParameters::Parameters Parameters;

        explicit OdeSolverJacobianTrimmer( OdeSolver<VD>& solver, const std::string& typeId ) :
            OdeSolverComponent<VD>( solver )
            {
            if( !typeId.empty() )
                this->setJacobianTrimmer( Factory< JacobianTrimmer<VD> >::newInstance( typeId ) );
            }

        OdeSolverJacobianTrimmer( const OdeSolverJacobianTrimmer<VD>& ) = delete;
        OdeSolverJacobianTrimmer<VD>& operator=( const OdeSolverJacobianTrimmer<VD>& ) = delete;

        void saveParameters( Parameters& parameters ) const {
            parameters["jtrim"] = this->jacobianTrimmer();
            }

        void loadParameters( const Parameters& parameters ) {
            OptionalParameters::maybeSetParameter(
                        parameters, "jtrim",
                        std::bind( &JacobianTrimmerHolder<VD>::setJacobianTrimmer, this, std::placeholders::_1 ) );
            }

        void addHelpOnParameters( Parameters& help ) {
            help["jtrim"] = OptionalParameters::appendNestedHelp(
                        this->jacobianTrimmer(),
                        "Jacobian trimmer to use" );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_SOLVER_H_

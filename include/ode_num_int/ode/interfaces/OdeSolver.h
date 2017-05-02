// OdeSolver.h

#ifndef _ODE_INTERFACES_ODESOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_INTERFACES_ODESOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./OdeRhs.h"
#include "../../timing/TimingStats.h"
#include "../../alg/interfaces/NewtonSolverIterationObservers.h"
#include "../../lu/LUFactorizerTimingStats.h"
#include "../../infra/def_getset.h"

#include <list>

namespace ctm {
namespace math {

template< class VD > class OdeSolver;

template< class VD >
using OdeSolverPreObservers = cxx::Observers<
    typename VD::value_type /*stepSize*/,
    const OdeSolver<VD>* >;

template< class VD >
struct OdeSolverPostObserverArg
    {
        CTM_DECL_THIS_CLASS_ALIAS( OdeSolverPostObserverArg<VD> )
        CTM_DECL_SIMPLE_CLASS_FIELD( typename VD::value_type, stepSize, setStepSize, 0 )
        CTM_DECL_SIMPLE_CLASS_FIELD( bool, stepAccepted, setStepAccepted, true )
        CTM_DECL_SIMPLE_CLASS_FIELD( bool, stepSizeChanged, setStepSizeChanged, false )
        CTM_DECL_SIMPLE_CLASS_FIELD( bool, stepTruncated, setStepTruncated, false )
        CTM_DECL_SIMPLE_CLASS_FIELD( typename VD::value_type, errorNorm, setErrorNorm, 0 )
        CTM_DECL_SIMPLE_CLASS_FIELD( unsigned int, izfTrunc, setIzfTrunc, ~0u )
        CTM_DECL_SIMPLE_CLASS_FIELD( int, transitionType, setTransitionType, 0 )
        CTM_DECL_SIMPLE_CLASS_FIELD( const OdeSolver<VD>*, solver, setSolver, nullptr )

        OdeSolverPostObserverArg() {}
        OdeSolverPostObserverArg(
                typename VD::value_type stepSize,
                bool stepAccepted,
                bool stepSizeChanged,
                bool stepTruncated,
                typename VD::value_type errorNorm,
                unsigned int izfTrunc,
                int transitionType,
                const OdeSolver<VD>* solver
                ) :
            m_stepSize( stepSize ),
            m_stepAccepted( stepAccepted ),
            m_stepSizeChanged( stepSizeChanged ),
            m_stepTruncated( stepTruncated ),
            m_errorNorm( errorNorm ),
            m_izfTrunc( izfTrunc ),
            m_transitionType( transitionType ),
            m_solver( solver )
            {}
    };

template< class VD >
using OdeSolverPostObservers = cxx::Observers< const OdeSolverPostObserverArg<VD>& >;

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
        LUFactorizerTimingStats luTimingStats;

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

} // end namespace math
} // end namespace ctm

#endif // _ODE_INTERFACES_ODESOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

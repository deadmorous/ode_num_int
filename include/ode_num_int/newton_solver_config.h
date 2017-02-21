// newton_solver_config.h

#ifndef _NEWTON_SOLVER_CONFIG_H_
#define _NEWTON_SOLVER_CONFIG_H_

#include "ode_simdyn.h"
#include <set>

namespace ctm {
namespace tools {
namespace cmd {
namespace numint {

class NewtonSolverComponents
    {
    public:
        NewtonSolverComponents(
            const std::shared_ptr< math::VectorMapping<math::VDSimDyn> >& eqnRhs,
            const std::shared_ptr< math::SimDynOde >& odeRhs,
            const std::shared_ptr< math::OdeTimeStepper<math::VDSimDyn> >& timeStepper,
            const std::shared_ptr< math::VectorNarrowingMapping<math::VDSimDyn> >& narrowedEqn,
            bool narrow,
            const std::shared_ptr< math::VectorReorderingMapping<math::VDSimDyn> >& reorderedEqn,
            bool reordered,
            const std::vector<unsigned int>& disabledVars,
            const std::shared_ptr< math::MappingNormalizer<math::VDSimDyn> >& eqnRhsNormalized,
            bool normalized ) :
                m_eqnRhs( eqnRhs ),
                m_odeRhs( odeRhs ),
                m_timeStepper( timeStepper ),
                m_narrowedEqn( narrowedEqn ),
                m_narrow( narrow ),
                m_reorderedEqn( reorderedEqn ),
                m_reordered( reordered ),
                m_disabledVars( disabledVars ),
                m_eqnRhsNormalized( eqnRhsNormalized ),
                m_normalized( normalized )
            {}


        std::shared_ptr< math::VectorMapping<math::VDSimDyn> > eqnRhs() const {
            return m_eqnRhs;
            }

        std::shared_ptr< math::SimDynOde > odeRhs() const {
            return m_odeRhs;
            }

        std::shared_ptr< math::OdeTimeStepper<math::VDSimDyn> > timeStepper() const {
            return m_timeStepper;
            }

        std::shared_ptr< math::VectorNarrowingMapping<math::VDSimDyn> > narrowedEqn() const {
            return m_narrowedEqn;
            }

        bool narrow() {
            return m_narrow;
            }

        std::shared_ptr< math::VectorReorderingMapping<math::VDSimDyn> > reorderedEqn() const {
            return m_reorderedEqn;
            }

        bool reordered() const {
            return m_reordered;
            }

        std::vector<unsigned int> disabledVars() const {
            return m_disabledVars;
            }

        std::shared_ptr< math::MappingNormalizer<math::VDSimDyn> > eqnRhsNormalized() const {
            return m_eqnRhsNormalized;
            }

        bool normalized() const {
            return m_normalized;
            }

    private:
        std::shared_ptr< math::VectorMapping<math::VDSimDyn> > m_eqnRhs;
        std::shared_ptr< math::SimDynOde > m_odeRhs;
        std::shared_ptr< math::OdeTimeStepper<math::VDSimDyn> > m_timeStepper;
        std::shared_ptr< math::VectorNarrowingMapping<math::VDSimDyn> > m_narrowedEqn;
        bool m_narrow;
        std::shared_ptr< math::VectorReorderingMapping<math::VDSimDyn> > m_reorderedEqn;
        bool m_reordered;
        std::vector<unsigned int> m_disabledVars;
        std::shared_ptr< math::MappingNormalizer<math::VDSimDyn> > m_eqnRhsNormalized;
        bool m_normalized;
    };

class NewtonSolverConfiguration
    {
    public:
        class ComponentParamBase
            {
            public:
                virtual ~ComponentParamBase() {}
                virtual FactoryBase::TypeId type() const = 0;
                virtual void setType( const FactoryBase::TypeId& type ) = 0;
                virtual const OptionalParameters::Parameters& parameters() const = 0;
                virtual std::string parameter( const std::string& name ) const = 0;
                virtual void setParameter( const std::string& name, const std::string& value ) = 0;
            };

        template< class Interface >
        class ComponentParam : public ComponentParamBase
            {
            public:
                ComponentParam() :
                    m_parametersKnown( true )
                    {}

                ComponentParam( const FactoryBase::TypeId& type ) :
                    m_type( type ),
                    m_parametersKnown( false )
                    {}

                FactoryBase::TypeId type() const {
                    return m_type;
                    }

                void setType( const FactoryBase::TypeId& type )
                    {
                    if( type.empty() ) {
                        m_type.clear();
                        m_parameters.clear();
                        m_parametersKnown = false;
                        }
                    else {
                        if( !Factory< Interface >::isTypeRegistered( type ) )
                            throw cxx::exception( std::string("The type '") + type + "' is not registered" );
                        m_type = type;
                        if( type.empty() ) {
                            m_parameters.clear();
                            m_parametersKnown = true;
                            }
                        else
                            m_parametersKnown = false;
                        }
                    }

                const OptionalParameters::Parameters& parameters() const
                    {
                    if( !m_parametersKnown ) {
                        ASSERT( !m_type.empty() );
                        m_parametersKnown = true;
                        m_parameters = Factory< Interface >::newInstance( m_type )->parameters();
                        }
                    return m_parameters;
                    }

                std::string parameter( const std::string& name ) const {
                    return const_cast< ComponentParam<Interface>* >( this )->parameterPriv( name );
                    }

                void setParameter( const std::string& name, const std::string& value ) {
                    parameterPriv( name ) = value;
                    }

                typename Factory< Interface >::InterfacePtr makeComponent()
                    {
                    if( m_type.empty() )
                        return typename Factory< Interface >::InterfacePtr();
                    auto result = Factory< Interface >::newInstance( m_type );
                    result->setParameters( m_parameters );
                    return result;
                    }

            private:
                FactoryBase::TypeId m_type;
                mutable OptionalParameters::Parameters m_parameters;
                mutable bool m_parametersKnown;

                OptionalParameters::Value& parameterPriv( const std::string& name )
                    {
                    parameters();
                    auto it = m_parameters.find( name );
                    if( it == m_parameters.end() )
                        throw cxx::exception( std::string("Parameter '") + name + "' is not found" );
                    else
                        return it->second;
                    }
            };

        NewtonSolverConfiguration();

        NewtonSolverComponents apply(
                math::NewtonSolver<math::VDSimDyn>& solver,
                mech::ISimulationContextCtl *sc,
                const std::set< unsigned int >& frozenVarIds,
                double odeInitTime,
                const math::VectorTemplate< math::VDSimDyn >& odeInitState,
                const math::VectorTemplate< math::VDSimDyn >& odeInitGuess,
                bool normalized = false );

        ComponentParam< math::OdeTimeStepper< math::VDSimDyn > > tstep;
        ComponentParam< math::NewtonDescentDirection< math::VDSimDyn > > ddir;
        ComponentParam< math::JacobianTrimmer< math::VDSimDyn > > jtrim;
        ComponentParam< math::NewtonLinearSearch< math::VDSimDyn > > linsearch;
        ComponentParam< math::ErrorEstimator< math::VDSimDyn > > errest;
        ComponentParam< math::NewtonRegularizationStrategy< math::VDSimDyn > > regularizer;
    };

} // end namespace numint
} // end namespace cmd
} // end namespace tools
} // end namespace ctm

#endif // _NEWTON_SOLVER_CONFIG_H_

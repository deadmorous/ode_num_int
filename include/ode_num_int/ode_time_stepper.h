// ode_time_stepper.h

#ifndef _ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "vector_mapping.h"

namespace ctm {
namespace math {

template< class VD > class OdeRhs;

template< class VD >
using OdeRhsPreObservers = cxx::Observers<
    typename VD::value_type /*time*/,
    const VectorTemplate< VD >& /*argument*/,
    const OdeRhs<VD>* /*odeRhs*/ >;

template< class VD >
using OdeRhsPostObservers = cxx::Observers<
    typename VD::value_type /*time*/,
    const VectorTemplate< VD >& /*argument*/,
    const VectorTemplate< VD >& /*value*/,
    const OdeRhs<VD>* /*odeRhs*/ >;

template< class VD >
class OdeRhs :
    public Factory< OdeRhs<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        OdeRhsPreObservers<VD> odeRhsPreObservers;
        OdeRhsPostObservers<VD> odeRhsPostObservers;

        V rhs( real_type time, const V& x ) const {
            V result( varCount() );
            rhs( result, time, x );
            return result;
            }
        V operator()( real_type time, const V& x ) const {
            return rhs( time, x );
            }

        virtual unsigned int secondOrderVarCount() const = 0;
        virtual unsigned int firstOrderVarCount() const = 0;
        virtual unsigned int zeroFuncCount() const {
            return 0;
            }

        virtual void rhs( V& dst, real_type time, const V& x ) const = 0;
        virtual void zeroFunctions( V& /*dst*/, real_type /*time*/, const V& /*x*/ ) const {}
        virtual void switchPhaseState( const int* /*transitions*/ ) {}
        virtual std::string describeZeroFunction( unsigned int /*index*/ ) const {
            return std::string();
            }

        virtual std::vector<unsigned int> varIds() const {
            return std::vector<unsigned int>( varCount(), 0 );
            }
        virtual void beforeStep( real_type /*time*/, const V& /*x*/ ) const {}
        virtual void beforeStep2( const V& /*rhs*/ ) const {}
        virtual bool truncateStep( real_type& /*tEnd*/, V& /*xEnd*/ ) const {
            return false;
        }

        unsigned int varCount() const {
            return 2*secondOrderVarCount() + firstOrderVarCount();
            }
    };
CTM_DEF_NOTIFIED_PROP_VD_TEMPLATE_CLASS(OdeRhsHolder, OdeRhs, odeRhs, setOdeRhs, onOdeRhsChanged, offOdeRhsChanged)

template< class VD >
class OdeRhsVectorMapping :
    public VectorMapping<VD>,
    public OdeRhsHolder<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeRhsVectorMapping() : m_time( 0 ) {}

        OdeRhsVectorMapping(
                const std::shared_ptr< OdeRhs<VD> >& rhs,
                real_type time ) :
            m_time( time )
            {
            this->setOdeRhs( rhs );
            }

        real_type time() const {
            return m_time;
            }

        void setTime( real_type time ) {
            m_time = time;
            }

        unsigned int inputSize() const {
            return this->odeRhs()->varCount();
            }

        unsigned int outputSize() const {
            return this->odeRhs()->varCount();
            }

        void map( V& dst, const V& x ) const {
            this->odeRhs()->rhs( dst, m_time, x );
            }

        std::vector<unsigned int> inputIds() const {
            return this->odeRhs()->varIds();
            }

        std::vector<unsigned int> outputIds() const {
            return this->odeRhs()->varIds();
            }

    private:
        real_type m_time;
    };

template< class VD >
inline std::shared_ptr< OdeRhsVectorMapping<VD> > makeOdeRhsVectorMapping(
        const std::shared_ptr< OdeRhs<VD> >& rhs,
        typename VD::value_type time )
    {
    return std::make_shared< OdeRhsVectorMapping<VD> >( rhs, time );
    }

template< class VD >
class OdeInitState :
    public Factory< OdeInitState<VD> >,
    public OptionalParameters
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        virtual real_type initialTime() const = 0;
        virtual V initialState() const = 0;
    };

template< class VD >
class SpecifiedOdeInitState :
    public OdeInitState<VD>,
    public FactoryMixin< SpecifiedOdeInitState<VD>, OdeInitState<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        SpecifiedOdeInitState() : m_initialTime( 0 ) {}

        real_type initialTime() const {
            return m_initialTime;
            }

        V initialState() const {
            return m_initialState;
            }

        Parameters parameters() const
            {
            Parameters result;
            int n = m_initialState.size();
            result["size"] = n;
            result["time"] = m_initialTime;
            for( int ivar=0; ivar<n; ++ivar )
                result[varName(ivar)] = m_initialState[ivar];
            return result;
            }

        void setParameters( const Parameters& parameters )
            {
            using namespace std;
            using namespace placeholders;
            OptionalParameters::maybeSetParameter( parameters, "size", bind( &SpecifiedOdeInitState<VD>::resize, this, _1 ) );
            OptionalParameters::maybeLoadParameter( parameters, "time", m_initialTime );
            auto n = m_initialState.size();
            for( decltype(n) ivar=0; ivar<n; ++ivar )
                OptionalParameters::maybeLoadParameter( parameters, varName(ivar), m_initialState[ivar] );
            }

    private:
        real_type m_initialTime;
        V m_initialState;

        void resize( int n ) {
            int n0 = m_initialState.size();
            if( n != n0 ) {
                m_initialState.resize( n );
                std::fill( m_initialState.begin() + n0, m_initialState.end(), real_type(0) );
                }
            }

        static std::string varName( unsigned int ivar ) {
            return std::string("x") + cxx::FormatInt(ivar);
            }
    };

template< class VD >
class FakeDatFileOdeInitState :
    public OdeInitState<VD>,
    public FactoryMixin< FakeDatFileOdeInitState<VD>, OdeInitState<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        real_type initialTime() const {
            // One should not really use the interface
            ASSERT( false );
            return 0;
            }

        V initialState() const {
            // One should not really use the interface
            ASSERT( false );
            return V();
            }
        Parameters parameters() const
            {
            Parameters result;
            result["file_name"] = m_initialStateFileName;
            return result;
            }

        void setParameters( const Parameters& parameters ) {
            OptionalParameters::maybeLoadParameter( parameters, "file_name", m_initialStateFileName );
            }

        virtual Parameters helpOnParameters() const
            {
            Parameters result;
            result["file_name"] = "Name of the file to use for the initial state (if none, the initial state is defined by simulation)";
            return result;
            }

    private:
        std::string m_initialStateFileName;
    };

template< class VD >
class OdeTimeStepper :
    public VectorMapping<VD>,
    public OdeRhsHolder<VD>,
    public Factory< OdeTimeStepper<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        virtual real_type initialTime() const = 0;
        virtual const V& initialState() const = 0;
        virtual void setInitialState( real_type initialTime, const V& initialState ) = 0;
        virtual real_type timeStep() const = 0;
        virtual void setTimeStep( real_type timeStep ) = 0;
        virtual V eqnInitialState() const = 0;
        virtual void computeOdeState( V& odeState, const V& eqnState ) const = 0;
        virtual void computeEqnState( V& eqnState, const V& odeState ) const = 0;
        virtual void beforeStep() const = 0;

        V computeOdeState( const V& eqnState ) const
            {
            V odeState;
            computeOdeState( odeState, eqnState );
            return odeState;
            }

        V computeEqnState( const V& odeState ) const
            {
            V eqnState;
            computeEqnState( eqnState, odeState );
            return eqnState;
            }
    };
CTM_DEF_PROP_VD_TEMPLATE_CLASS(
        OdeTimeStepperHolder, OdeTimeStepper,
        timeStepper, setTimeStepper )



template< class VD >
class OdeTimeStepperEuler :
    public OdeTimeStepper< VD >,
    public FactoryMixin< OdeTimeStepperEuler<VD>, OdeTimeStepper<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        OdeTimeStepperEuler() :
            m_alpha( 1 ),
            m_h( 0.1 ),
            m_t0( 0 ),
            m_f0scaledValid( false )
            {}

        // VectorMapping interface
        unsigned int inputSize() const
            {
            auto rhs = this->odeRhs();
            ASSERT( rhs );
            return rhs->firstOrderVarCount() + rhs->secondOrderVarCount();
            }

        unsigned int outputSize() const
            {
            auto rhs = this->odeRhs();
            ASSERT( rhs );
            return rhs->firstOrderVarCount() + rhs->secondOrderVarCount();
            }

        void map( V& dst, const V& x ) const
            {
            ASSERT( x.size() == inputSize() );
            this->vectorMappingPreObservers( x, this );

            computeF0Scaled();

            dst.resize( outputSize() );
            auto rhs = this->odeRhs();
            auto nd = rhs->secondOrderVarCount();
            auto n1 = rhs->firstOrderVarCount();

            // Compute U
            V stateBuf( 2*nd + n1 );
            expandState( stateBuf, x );

            // Compute ODE rhs
            V rhsBuf( rhs->varCount() );
            rhs->rhs( rhsBuf, m_t0+m_h, stateBuf );

            // Compute equation rhs X - X0 - h*(1-alpha)*F0 - h*alpha*F
            dst = x;
            dst -= m_x0.block( nd, nd+n1 );
            dst += m_f0scaled.block( nd, nd+n1 );
            dst += rhsBuf.block( nd, nd+n1 ).scaled( -m_h*m_alpha );

            this->vectorMappingPostObservers( x, dst, this );
            }

        virtual std::vector<unsigned int> inputIds() const {
            return varIds();
            }

        virtual std::vector<unsigned int> outputIds() const {
            return varIds();
            }

        // OdeTimeStepper interface
        real_type initialTime() const {
            return m_t0;
            }

        const V& initialState() const {
            return m_x0;
            }

        void setInitialState( real_type initialTime, const V& initialState )
            {
            m_x0 = initialState;
            m_t0 = initialTime;
            m_f0scaledValid = false;
            }

        real_type timeStep() const {
            return m_h;
            }

        void setTimeStep( real_type timeStep )
            {
            ASSERT( timeStep > 0 );
            m_h = timeStep;
            m_f0scaledValid = false;
            }

        V eqnInitialState() const
            {
            auto rhs = this->odeRhs();
            auto nd = rhs->secondOrderVarCount();
            auto n1 = rhs->firstOrderVarCount();
            return m_x0.block( nd, nd+n1 );
            }

        void computeOdeState( V& odeState, const V& eqnState ) const
            {
            auto rhs = this->odeRhs();
            odeState.resize( rhs->varCount() );
            expandState( odeState, eqnState );
            }

        V computeOdeState( const V& eqnState ) const {
            return OdeTimeStepper< VD >::computeOdeState( eqnState );
            }

        virtual void computeEqnState( V& eqnState, const V& odeState ) const
            {
            auto rhs = this->odeRhs();
            auto nd = rhs->secondOrderVarCount();
            auto n1 = rhs->firstOrderVarCount();
            eqnState = odeState.block( nd, nd+n1 );
            }

        V computeEqnState( const V& odeState ) const {
            return OdeTimeStepper< VD >::computeEqnState( odeState );
            }

        void beforeStep() const {
            this->odeRhs()->beforeStep( m_t0, m_x0 );
            }

        // Additional parameters
        real_type implicitnessParameter() const {
            return m_alpha;
            }

        void setImplicitnessParameter( real_type implicitnessParameter )
            {
            m_alpha = implicitnessParameter;
            m_f0scaledValid = false;
            }

        OptionalParameters::Parameters parameters() const
            {
            OptionalParameters::Parameters result;
            result["h"] = m_h;
            result["alpha"] = m_alpha;
            return result;
            }

        void setParameters( const OptionalParameters::Parameters & parameters )
            {
            using namespace std;
            using namespace placeholders;
            OptionalParameters::maybeSetParameter( parameters, "h", bind( &OdeTimeStepperEuler<VD>::setTimeStep, this, _1 ) );
            OptionalParameters::maybeSetParameter( parameters, "alpha", bind( &OdeTimeStepperEuler<VD>::setImplicitnessParameter, this, _1 ) );
            }

    private:
        real_type m_alpha;
        real_type m_h;
        real_type m_t0;
        V m_x0;
        mutable V m_f0scaled;
        mutable bool m_f0scaledValid;

        void expandState( V& dst, const V& x ) const
            {
            auto nd = this->odeRhs()->secondOrderVarCount();
            auto n1 = this->odeRhs()->firstOrderVarCount();
            ASSERT( x.size() == nd+n1 );
            ASSERT( dst.size() == 2*nd+n1 );

            // Compute U
            auto U = dst.block( 0, nd );
            U = m_x0.block( 0, nd );
            U += m_x0.block( nd, nd ).scaled( m_h*(1-m_alpha) );
            U += x.block( 0, nd ).scaled( m_h*m_alpha );

            // Copy V and Z to the end of dst
            dst.block( nd, nd+n1 ) = x;
            }

        void computeF0Scaled() const
            {
            if( !m_f0scaledValid ) {
                this->odeRhs()->rhs( m_f0scaled, m_t0, m_x0 );
                m_f0scaled *= -m_h*(1-m_alpha);
                m_f0scaledValid = true;
                }
            }

        std::vector<unsigned int> varIds() const
            {
            auto rhs = this->odeRhs();
            auto varIds = rhs->varIds();
            return std::vector<unsigned int>( varIds.begin() + rhs->secondOrderVarCount(), varIds.end() );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_TIME_STEPPER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

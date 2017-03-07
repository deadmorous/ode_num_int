// OdeStepMappingEuler.h

#ifndef _ODESOLVER_ODESTEPMAPPINGEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESTEPMAPPINGEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeStepMapping.h"

namespace ctm {
namespace math {

template< class VD >
class OdeStepMappingEuler :
    public OdeStepMapping< VD >,
    public FactoryMixin< OdeStepMappingEuler<VD>, OdeStepMapping<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename VD::value_type real_type;

        OdeStepMappingEuler() :
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

        // OdeStepMapping interface
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
            return OdeStepMapping< VD >::computeOdeState( eqnState );
            }

        virtual void computeEqnState( V& eqnState, const V& odeState ) const
            {
            auto rhs = this->odeRhs();
            auto nd = rhs->secondOrderVarCount();
            auto n1 = rhs->firstOrderVarCount();
            eqnState = odeState.block( nd, nd+n1 );
            }

        V computeEqnState( const V& odeState ) const {
            return OdeStepMapping< VD >::computeEqnState( odeState );
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
            OptionalParameters::maybeSetParameter( parameters, "h", bind( &OdeStepMappingEuler<VD>::setTimeStep, this, _1 ) );
            OptionalParameters::maybeSetParameter( parameters, "alpha", bind( &OdeStepMappingEuler<VD>::setImplicitnessParameter, this, _1 ) );
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

#endif // _ODESOLVER_ODESTEPMAPPINGEULER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

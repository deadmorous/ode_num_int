// SpecifiedOdeInitState.h

#ifndef _ODESOLVER_SPECIFIEDODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_SPECIFIEDODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeInitState.h"

namespace ctm {
namespace math {

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

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_SPECIFIEDODEINITSTATE_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

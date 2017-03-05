// ode_model_sliding_point.h

#ifndef _ODE_MODEL_SLIDING_POINT_H_
#define _ODE_MODEL_SLIDING_POINT_H_

#include "../ode_solver/OdeRhs.h"
#include "ctmstd/m_const.h"

namespace ctm {
namespace testmodels {

using namespace math;

template< class VD >
class SlidingPoint :
    public OdeRhs< VD >,
    public FactoryMixin< SlidingPoint<VD>, OdeRhs<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        enum FrictionType { Atan, Tabular, Linear };

        explicit SlidingPoint() :
            m_frictionType( Atan ),
            m_mass( 1 ),
            m_f0( 1 ),
            m_v0( 1 ),
            m_F1( 10 ),
            m_F2( 10 ),
            m_omega( 100 )
            {}

        virtual unsigned int secondOrderVarCount() const {
            return 2;
            }

        virtual unsigned int firstOrderVarCount() const {
            return 0;
            }

        virtual void rhs( V& dst, real_type time, const V& x ) const
            {
            this->odeRhsPreObservers( time, x, this );
            dst.resize( 4 );
            auto v1 = x[2];
            auto v2 = x[3];
            auto v = sqrt( v1*v1 + v2*v2 );
            real_type R1, R2;
            std::tie(R1, R2) = frictionForce( v1, v2 );
            auto F1 = m_F1 + R1;
            auto F2 = m_F2*sin(m_omega*time) + R2;
            dst[0] = v1;
            dst[1] = v2;
            dst[2] = F1/m_mass;
            dst[3] = F2/m_mass;
            this->odeRhsPostObservers( time, x, dst, this );
            }

        Parameters parameters() const
            {
            Parameters result;
            result["friction"] = fromEnum(m_frictionType);
            result["mass"] = m_mass;
            result["f0"] = m_f0;
            result["v0"] = m_v0;
            result["F1"] = m_F1;
            result["F2"] = m_F2;
            result["omega"] = m_omega;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeSetParameter( parameters, "friction", [this]( const std::string& s ) {
                m_frictionType = SlidingPoint<VD>::toEnum(s);
                } );
            this->maybeLoadParameter( parameters, "mass", m_mass );
            this->maybeLoadParameter( parameters, "f0", m_f0 );
            this->maybeLoadParameter( parameters, "v0", m_v0 );
            this->maybeLoadParameter( parameters, "F1", m_F1 );
            this->maybeLoadParameter( parameters, "F2", m_F2 );
            this->maybeLoadParameter( parameters, "omega", m_omega );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["friction"] = "Friction type, one of 'atan', 'tabular, 'linear'";
            result["mass"] = "The mass of the point";
            result["f0"] = "Saturation value for the friction force";
            result["v0"] = "Saturation speed for the friction force";
            result["F1"] = "Constant applied force in direction 1";
            result["F2"] = "Amplitude of the applied harmonic force in direction 2";
            result["omega"] = "Frequency of the applied harmonic force in direction 2";
            return result;
            }

        std::string helpOnType() const {
            return "Point moving on plane with friction under two applied force.\n"
                   "A constant force is applied in direction 1, and a harmonic\n"
                   "force is applied in direction 2.";
            }

    private:
        FrictionType m_frictionType;
        real_type m_mass;
        real_type m_f0;
        real_type m_v0;
        real_type m_F1;
        real_type m_F2;
        real_type m_omega;

        std::tuple<real_type, real_type> frictionForce( real_type v1, real_type v2 ) const
            {
            auto v = sqrt( v1*v1 + v2*v2 );
            real_type e1, e2;
            if( v > 0 ) {
                e1 = v1/v;
                e2 = v2/v;
                }
            else
                e1 = e2 = 0;
            real_type f;
            switch( m_frictionType ) {
                case Atan:
                    f = 2./M_PI * m_f0 * atan( v/m_v0 );
                    break;
                case Tabular:
                    f = v < m_v0 ?   m_f0 * v/m_v0 :   m_f0;
                    break;
                case Linear:
                    f = m_f0 * v/m_v0;
                    break;
                default:
                    ASSERT( false );
                    throw cxx::exception("Invalid friction type");
                }
            return std::make_tuple( -f*e1, -f*e2 );
            }


        static std::vector< std::pair<FrictionType, std::string> > enumText()
            {
            return {
                std::make_pair(Atan, "atan"),
                std::make_pair(Tabular, "tabular"),
                std::make_pair(Linear, "linear")
                };
            }

        static std::string fromEnum( FrictionType ft )
            {
            for( const auto& p : enumText() )
                if ( ft == p.first )
                    return p.second;
            throw cxx::exception("Unknown enum value");
            }

        static FrictionType toEnum( const std::string& s )
            {
            for( const auto& p : enumText() )
                if ( s == p.second )
                    return p.first;
            throw cxx::exception( std::string("Unknown enum value '") + s + "'" );
            }
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_MODEL_SLIDING_POINT_H_

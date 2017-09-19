// BouncingBall.h

#ifndef _ODE_TEST_MODELS_BOUNCINGBALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_TEST_MODELS_BOUNCINGBALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../interfaces/OdeRhs.h"
#include "../../util/m_const.h"

namespace ctm {
namespace testmodels {

using namespace math;

template< class VD >
class BouncingBall :
    public OdeRhs< VD >,
    public FactoryMixin< BouncingBall<VD>, OdeRhs<VD> >
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;
        typedef OptionalParameters::Parameters Parameters;

        enum FrictionType { Atan, Tabular, Linear };

        explicit BouncingBall() :
            m_g( 9.8 ),
            m_recoveryFactor( 0.9 ),
            m_stickSpeed( 1e-5 ),
            m_sticking( false )
            {}

        unsigned int secondOrderVarCount() const {
            return 1;
            }

        unsigned int firstOrderVarCount() const {
            return 0;
            }

        unsigned int zeroFuncCount() const {
            return 1;
            }

        virtual std::vector<unsigned int> zeroFuncFlags() const {
            return std::vector<unsigned int>( 1, OdeRhs<VD>::PlusMinus );
        }

        void rhs( V& dst, real_type time, const V& x ) const
            {
            this->odeRhsPreObservers( time, x, this );
            dst.resize( 2 );
            if( m_sticking )
                dst[0] = dst[1] = 0;
            else {
                dst[0] = x[1];
                dst[1] = -m_g;
                }
            this->odeRhsPostObservers( time, x, dst, this );
            }

        void zeroFunctions( V& dst, real_type /*time*/, const V& x ) const
            {
            dst.resize( 1 );
            dst[0] = x[0];
            }

        void switchPhaseState( const int* transitions, real_type /*time*/, V& x )
            {
            auto stick = [&, this]() {
                x[0] = x[1] = 0;
                m_sticking = true;
                };

            if( fabs(x[1]) < m_stickSpeed )
                stick();
            else if( x[1] < 0 ) {
                // Falling down, there was no collision just a moment before
                x[1] *= -m_recoveryFactor;
                }
            else
                // There was a collision just a moment before, because the speed is positive.
                // If we let the point go up, there will be a collision again because at the end of
                // next time step the point will be below the plane again, just as it is now.
                // As a result, there will be an infinite loop.
                // Therefore, the best we can do now is to consider the point stick to the plane.
                stick();
            }

        std::string describeZeroFunction( unsigned int /*index*/ ) const {
            return "Height of the point over the plane";
            }

        Parameters parameters() const
            {
            Parameters result;
            result["gravity_acceleration"] = m_g;
            result["recovery_factor"] = m_recoveryFactor;
            result["stick_speed"] = m_stickSpeed;
            return result;
            }

        void setParameters( const Parameters & parameters )
            {
            this->maybeLoadParameter( parameters, "gravity_acceleration", m_g );
            this->maybeLoadParameter( parameters, "recovery_factor", m_recoveryFactor );
            this->maybeLoadParameter( parameters, "stick_speed", m_stickSpeed );
            }

        Parameters helpOnParameters() const
            {
            Parameters result;
            result["gravity_acceleration"] = "Gravity acceleration";
            result["recovery_factor"] = "Impact recovery factor";
            result["stick_speed"] = "Normal speed threshold for sticking";
            return result;
            }

        std::string helpOnType() const {
            return "Point falling onto a horizontal plane under the gravity force.\n"
                   "Point-plane collision leads to an impact, so the point is reflected;\n"
                   "its impulse decreases according to a recovery factor.";
            }

    private:
        real_type m_g;
        real_type m_recoveryFactor;
        real_type m_stickSpeed;

        bool m_sticking;
    };

} // end namespace testmodels
} // end namespace ctm

#endif // _ODE_TEST_MODELS_BOUNCINGBALL_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

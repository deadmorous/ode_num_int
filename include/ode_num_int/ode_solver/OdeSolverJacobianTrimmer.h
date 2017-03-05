// OdeSolverJacobianTrimmer.h

#ifndef _ODESOLVER_ODESOLVERJACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODESOLVERJACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "alg_solver/JacobianTrimmer.h"
#include "OdeSolver.h"

namespace ctm {
namespace math {

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

#endif // _ODESOLVER_ODESOLVERJACOBIANTRIMMER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

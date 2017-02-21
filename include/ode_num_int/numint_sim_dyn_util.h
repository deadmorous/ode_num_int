// numint_sim_dyn_util.h

#ifndef _NUMINT_SIM_DYN_UTIL_H_
#define _NUMINT_SIM_DYN_UTIL_H_

#include "sim_dyn_util/sim_dyn_util.h"

namespace ctm {
namespace mech {

class NumIntSimDynUtil
    {
    public:
        NumIntSimDynUtil();

        static NumIntSimDynUtil fromGlobals( inproc::om::ISomething *globals );
        static NumIntSimDynUtil fromSimulation( inproc::om::ISomething *sim );
        static NumIntSimDynUtil fromSimContext( ISimulationContext *sc );
        static NumIntSimDynUtil *current();
        static void setCurrent( NumIntSimDynUtil *current );
        static WSISimulationContextCtl currentSimContext();

        inproc::om::WBISomething simulation() const;
        WSISimulationContextCtl simContext() const;
        std::vector< const ISimulationObjectBase* > simObjects() const;
        std::vector< unsigned int > dofReductionMapOld2New() const;
        std::vector< unsigned int > dofReductionMapNew2Old() const;
        std::vector< unsigned int > varReductionMapOld2New() const;
        std::vector< unsigned int > varReductionMapNew2Old() const;
        unsigned int dofCount() const;
        unsigned int reducedDofCount() const;
        unsigned int reducedVarCount() const;
        std::vector< unsigned int > varIds() const;
        std::vector< unsigned int > reducedVarIds() const;

        static inproc::om::WBISomething findSimulation( inproc::om::ISomething *globals );

    private:
        inproc::om::WBISomething m_sim;
        static NumIntSimDynUtil *m_current;
    };

} // end namespace mech
} // end namespace ctm

#endif // _NUMINT_SIM_DYN_UTIL_H_

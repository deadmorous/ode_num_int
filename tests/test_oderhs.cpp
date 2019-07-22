#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "ode_num_int/OdeRhs.h"
#include "ode_num_int/BouncingBall.h"
#include "ode_num_int/Oscillator.h"
#include "ode_num_int/SlidingPoint.h"
#include "ode_num_int/StiffOde1D.h"
#include "ode_num_int/CoupledNonlinearOscillators.h"
#include "ode_num_int/OptionalParameters.h"

using namespace ctm::testmodels;


TEST(OdeRhs, TestmodelsTest)
{
    BouncingBall<VectorData<double>> BouncingBall;
    ctm::OptionalParameters::Parameters ball, newParam, gravAccel; newParam["recovery_factor"] = 11; ctm::OptionalParameters G;
    gravAccel["gravity_acceleration"] = 9.8; /*auto b = gravAccel["gravity_acceleration"];*/ double a = std::stod(gravAccel["gravity_acceleration"]);
    ball = BouncingBall.parameters(); /*double a = ball["gravity_acceleration"]; */ double b = std::stod(ball["gravity_acceleration"]);
    ASSERT_EQ(a, b);
//    BouncingBall.setParameters(newParam);
//    ASSERT_EQ(ball["recobery_factor"], 11);
}

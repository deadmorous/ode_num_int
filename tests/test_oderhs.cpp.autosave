#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "ode_num_int/OdeRhs.h"
#include "ode_num_int/BouncingBall.h"

using namespace ctm::testmodels;


TEST(OdeRhs, TestmodelsTest)
{
    BouncingBall<VectorData<double>> BouncingBall;
    ctm::OptionalParameters::Parameters ball, newBall, forComparsion;
    newBall["recovery_factor"] = 0.5;
    forComparsion["gravity_acceleration"] = 9.8; forComparsion["recovery_factor"] = 0.5;

    ball = BouncingBall.parameters();
    double a = std::stod(forComparsion["gravity_acceleration"]);
    double b = std::stod(ball["gravity_acceleration"]);
    ASSERT_EQ(a, b);

    BouncingBall.setParameters(newBall);
    ball = BouncingBall.parameters();
    a = std::stod(forComparsion["recovery_factor"]);
    b = std::stod(ball["recovery_factor"]);
    ASSERT_EQ(a, b);
}

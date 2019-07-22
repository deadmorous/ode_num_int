#include <gtest/gtest.h>
#include <iostream>
#include "ode_num_int/ExtrapolatorStepSequenceBulirsch.h"
#include "ode_num_int/ExtrapolatorStepSequenceHarmonic.h"
#include "ode_num_int/ExtrapolatorStepSequenceRomberg.h"

using namespace ctm::math;

class Method {

    ExtrapolatorStepSequenceBulirsch Bulirsch;
    ExtrapolatorStepSequenceRomberg Romberg;
    ExtrapolatorStepSequenceHarmonic Harmonic;
    std::vector<int> vB = {1, 2, 3, 4, 6, 8}, vR = {1, 2, 4 ,8 ,16 ,32}, vH =  {1, 2, 3 ,4 ,5 ,6};

public:

    void resetAll() {Bulirsch.reset(); Romberg.reset(); Harmonic.reset();}

    int MethodNum(unsigned int n)
    {
        std::vector<int> v(3);
        v[1] = Bulirsch.next(); v[2] = Romberg.next(); v[3] = Harmonic.next();
        return v[n];
    }

    std::vector<int> SequenceNum(unsigned int n)
    {
        std::vector<int> vn;
        if (n == 1) {return vB;} if (n == 2) {return vR;} else {return vH;}
    }
};

TEST(ExtrapolatorStepSequence, ExtrapolatorStepSequenceTest)
{
    Method Met;
    std::vector<int> v1 , v2;
    for (unsigned int i = 1; i <= 3; i++)
    {
        Met.resetAll();
        for (int n = 0; n <= 5; n++)
        {
            auto a = Met.MethodNum(i); v1.insert(v1.end(), a);
        }
        v2 = Met.SequenceNum(i);
        ASSERT_TRUE(v1 == v2);
        v1.clear(); v2.clear();
    }
}

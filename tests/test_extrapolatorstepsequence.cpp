#include <gtest/gtest.h>
#include <iostream>
#include "ode_num_int/ExtrapolatorStepSequenceBulirsch.h"
#include "ode_num_int/ExtrapolatorStepSequenceHarmonic.h"
#include "ode_num_int/ExtrapolatorStepSequenceRomberg.h"

using namespace ctm::math;

template <class Seq>
struct ReferenceSequenceProvider
{
    static std::vector<int> referenceSequence();
};

template <> std::vector<int> ReferenceSequenceProvider<ExtrapolatorStepSequenceBulirsch>::referenceSequence() { return {1, 2, 3, 4, 6, 8}; }
template <> std::vector<int> ReferenceSequenceProvider<ExtrapolatorStepSequenceRomberg>::referenceSequence() { return {1, 2, 4 ,8 ,16 ,32}; }
template <> std::vector<int> ReferenceSequenceProvider<ExtrapolatorStepSequenceHarmonic>::referenceSequence() { return {1, 2, 3 ,4 ,5 ,6}; }

template<typename Seq>
struct TestExtrapolatorStepSequence : public testing::Test
{
    using MyParamType = Seq;
    static void test()
        {
            MyParamType s;
            for (auto i=0; i<3; ++i) {
                for (auto v : ReferenceSequenceProvider<MyParamType>::referenceSequence())
                    ASSERT_EQ(v, s.next());
                s.reset();
            }
        }
};

using ExtrapolatorStepSequences = testing::Types<
    ExtrapolatorStepSequenceBulirsch,
    ExtrapolatorStepSequenceRomberg,
    ExtrapolatorStepSequenceHarmonic>;
TYPED_TEST_SUITE(TestExtrapolatorStepSequence, ExtrapolatorStepSequences);

TYPED_TEST(TestExtrapolatorStepSequence, ExtrapolatorStepSequence)
{
    using MyParamType  = typename TestFixture::MyParamType;
    TestExtrapolatorStepSequence<MyParamType>::test();
}

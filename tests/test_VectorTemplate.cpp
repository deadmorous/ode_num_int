#include <gtest/gtest.h>
#include <algorithm>
#include <cstdlib>
#include "ode_num_int/VectorTemplate.h"

using namespace ctm::math;


TEST(VectorTemplate, IsInitiallyZero) {
    Vector<double> v(10);
    for (auto x : v)
        EXPECT_EQ(x, 0);
}

TEST(VectorTemplate, CopiesCtorInput) {
    std::vector<double> input_vector(10);
    std::generate(input_vector.begin(), input_vector.end(), [](){ return 1.23*(rand() % 100); });

    VectorData<double> input_VectorData(input_vector);
    ASSERT_EQ(input_VectorData.size(), input_vector.size());
    ASSERT_TRUE(std::equal(input_VectorData.begin(), input_VectorData.end(), input_vector.begin()));

    Vector<double> v(input_VectorData);
    ASSERT_EQ(v.size(), input_vector.size());
    ASSERT_TRUE(std::equal(v.begin(), v.end(), input_vector.begin()));
}
// TODO: More tests covering VectorTemplate interface

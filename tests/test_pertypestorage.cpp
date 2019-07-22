#include <gtest/gtest.h>
#include "ode_num_int/PerTypeStorage.h"

using namespace ctm;

TEST(PerTypeStorage, PerTypeStorageTest){
    PerTypeStorage::value<int, int>() = 200;
    PerTypeStorage::value<int, double>() = 15.6;
    auto x = PerTypeStorage::value<int, int>(); auto y = PerTypeStorage::value<int, double>();
    ASSERT_EQ(x, 200); ASSERT_EQ(y, 15.6);
}

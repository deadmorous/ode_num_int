// LUFactorizerTimingStats.h

#ifndef _LU_LUFACTORIZERTIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _LU_LUFACTORIZERTIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../TimingStats.h"

namespace ctm {
namespace math {

struct LUFactorizerTimingStats
    {
    sys::TimingStats factorizeTiming;
    sys::TimingStats solveTiming;
    sys::TimingStats setMatrixTiming;
    sys::TimingStats setMatrixFastTiming;
    sys::TimingStats updateTiming;
    LUFactorizerTimingStats& operator+=( const LUFactorizerTimingStats& that ) {
        factorizeTiming += that.factorizeTiming;
        solveTiming += that.solveTiming;
        setMatrixTiming += that.setMatrixTiming;
        setMatrixFastTiming += that.setMatrixFastTiming;
        updateTiming += that.updateTiming;
        return *this;
        }
    LUFactorizerTimingStats operator+( const LUFactorizerTimingStats& that ) const {
        LUFactorizerTimingStats result = *this;
        result += that;
        return result;
        }
    };

} // end namespace math
} // end namespace ctm

#endif // _LU_LUFACTORIZERTIMINGSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

// ErrorStats.h

#ifndef _UTIL_ERRORSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_ERRORSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../la/VectorTemplate.h"

namespace ctm {
namespace math {

template< class X >
class ErrorStats
    {
    public:
        typedef X value_type;

        ErrorStats() : m_sampleCount( 0 ) {}

        ErrorStats( const ErrorStats& ) = default;

        template< class Y >
        explicit ErrorStats( const Y& zero ) :
            m_errorMax( zero ),
            m_errorSum( zero ),
            m_errorSquareSum( zero ),
            m_sampleCount( 0 )
            {}

        ErrorStats(
                const X& errorMax,
                const X& errorSum,
                const X& errorSquareSum,
                unsigned int sampleCount ) :
            m_errorMax( errorMax ),
            m_errorSum( errorSum ),
            m_errorSquareSum( errorSquareSum ),
            m_sampleCount( sampleCount )
            {}

        X errorMaxValue() const {
            return m_errorMax;
            }

        X errorSum() const {
            return m_errorSum;
            }

        X errorSquareSum() const {
            return m_errorSquareSum;
            }

        X errorMeanValue() const
            {
            ASSERT( m_sampleCount > 0 );
            X result = m_errorSum;
            result *= 1. / m_sampleCount;
            return result;
            }

        X errorMeanSqareDev() const
            {
            ASSERT( m_sampleCount > 0 );
            X result = ( math::sqr( m_errorSum ) + m_errorSquareSum );
            result *= 1. / m_sampleCount;
            return std::sqrt( result );
            }

        unsigned int sampleCount() const {
            return m_sampleCount;
            }

        void addSample( const X& error )
            {
            ++m_sampleCount;
            auto e = std::fabs( error );
            m_errorMax = std::max( e, m_errorMax );
            m_errorSum += e;
            m_errorSquareSum += math::sqr( e );
            }

        ErrorStats<X>& operator+=( const ErrorStats<X>& that )
            {
            m_errorMax = std::max( m_errorMax, that.m_errorMax );
            m_errorSum += that.m_errorSum;
            m_errorSquareSum += that.m_errorSquareSum;
            m_sampleCount += that.m_sampleCount;
            return *this;
            }

        ErrorStats<X> operator+( const ErrorStats<X>& that ) const
            {
            ErrorStats<X> result = *this;
            result += that;
            return result;
            }

    private:
        X m_errorMax;
        X m_errorSum;
        X m_errorSquareSum;
        unsigned int m_sampleCount;
    };

} // end namespace math
} // end namespace ctm

#endif // _UTIL_ERRORSTATS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

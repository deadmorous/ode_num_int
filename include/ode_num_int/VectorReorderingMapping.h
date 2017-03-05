// VectorReorderingMapping.h

#ifndef _VECTORREORDERINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _VECTORREORDERINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "VectorMapping.h"

namespace ctm {
namespace math {

template< class VD >
class VectorReorderingMapping :
    public VectorMapping<VD>,
    public MappingHolder<VD>,
    public Factory< VectorReorderingMapping<VD> >
    {
    public:
        typedef VectorTemplate<VD> V;
        typedef typename V::value_type real_type;

        std::vector< unsigned int > inputOrdering() const
            {
            maybeInit();
            return m_inputOrdering;
            }

        void setInputOrdering( const std::vector< unsigned int >& inputOrdering ) {
            m_inputOrdering = inputOrdering;
            }

        std::vector< unsigned int > outputOrdering() const {
            maybeInit();
            return m_outputOrdering;
            }

        void setOutputOrdering( const std::vector< unsigned int >& outputOrdering ) {
            m_outputOrdering = outputOrdering;
            }

        unsigned int inputSize() const
            {
            maybeInit();
            return this->mapping()->inputSize();
            }

        unsigned int outputSize() const {
            maybeInit();
            return this->mapping()->outputSize();
            }

        template< class Container >
        void orderInput( Container& dst, const Container& x ) const
            {
            maybeInit();
            order( dst, x, m_inputOrdering );
            }

        template< class Container >
        Container orderInput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            order( dst, x, m_inputOrdering );
            return dst;
            }

        template< class Container >
        void unorderInput( Container& dst, const Container& x ) const
            {
            maybeInit();
            unorder( dst, x, m_inputOrdering );
            }

        template< class Container >
        Container unorderInput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            unorder( dst, x, m_inputOrdering );
            return dst;
            }

        template< class Container >
        void orderOutput( Container& dst, const Container& x ) const
            {
            maybeInit();
            order( dst, x, m_outputOrdering );
            }

        template< class Container >
        Container orderOutput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            order( dst, x, m_outputOrdering );
            return dst;
            }

        template< class Container >
        void unorderOutput( Container& dst, const Container& x ) const
            {
            maybeInit();
            unorder( dst, x, m_outputOrdering );
            }

        template< class Container >
        Container unorderOutput( const Container& x ) const
            {
            Container dst;
            maybeInit();
            unorder( dst, x, m_outputOrdering );
            return dst;
            }

        void map( V& dst, const V& x ) const
            {
            V xu, dstu;
            unorderInput( xu, x );
            this->mapping()->map( dstu, xu );
            orderOutput( dst, dstu );
            }

        std::vector<unsigned int> inputIds() const
            {
            std::vector<unsigned int> io;
            orderInput( io, this->mapping()->inputIds() );
            return io;
            }

        std::vector<unsigned int> outputIds() const {
            std::vector<unsigned int> io;
            orderOutput( io, this->mapping()->outputIds() );
            return io;
            }

    protected:
        virtual void maybeInit() {}

        void maybeInit() const {
            const_cast< VectorReorderingMapping<VD>* >( this )->maybeInit();
            }

    private:
        std::vector< unsigned int > m_inputOrdering;
        std::vector< unsigned int > m_outputOrdering;

        template< class Container >
        void order( Container& dst, const Container& x, const std::vector<unsigned int> ordering ) const
            {
            auto n = ordering.size();
            ASSERT( x.size() == n );
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[i] = x[ordering[i]];
            }

        template< class Container >
        void unorder( Container& dst, const Container& x, const std::vector<unsigned int> ordering ) const
            {
            auto n = ordering.size();
            ASSERT( x.size() == n );
            dst.resize( n );
            for( decltype(n) i=0; i<n; ++i )
                dst[ordering[i]] = x[i];
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _VECTORREORDERINGMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

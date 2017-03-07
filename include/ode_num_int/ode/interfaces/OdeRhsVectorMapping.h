// OdeRhsVectorMapping.h

#ifndef _ODESOLVER_ODERHSVECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODESOLVER_ODERHSVECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../../alg/interfaces/VectorMapping.h"
#include "OdeRhs.h"

namespace ctm {
namespace math {

template< class VD >
class OdeRhsVectorMapping :
    public VectorMapping<VD>,
    public OdeRhsHolder<VD>
    {
    public:
        typedef VectorTemplate< VD > V;
        typedef typename V::value_type real_type;

        OdeRhsVectorMapping() : m_time( 0 ) {}

        OdeRhsVectorMapping(
                const std::shared_ptr< OdeRhs<VD> >& rhs,
                real_type time ) :
            m_time( time )
            {
            this->setOdeRhs( rhs );
            }

        real_type time() const {
            return m_time;
            }

        void setTime( real_type time ) {
            m_time = time;
            }

        unsigned int inputSize() const {
            return this->odeRhs()->varCount();
            }

        unsigned int outputSize() const {
            return this->odeRhs()->varCount();
            }

        void map( V& dst, const V& x ) const {
            this->odeRhs()->rhs( dst, m_time, x );
            }

        std::vector<unsigned int> inputIds() const {
            return this->odeRhs()->varIds();
            }

        std::vector<unsigned int> outputIds() const {
            return this->odeRhs()->varIds();
            }

    private:
        real_type m_time;
    };

template< class VD >
inline std::shared_ptr< OdeRhsVectorMapping<VD> > makeOdeRhsVectorMapping(
        const std::shared_ptr< OdeRhs<VD> >& rhs,
        typename VD::value_type time )
    {
    return std::make_shared< OdeRhsVectorMapping<VD> >( rhs, time );
    }

} // end namespace math
} // end namespace ctm

#endif // _ODESOLVER_ODERHSVECTORMAPPING_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

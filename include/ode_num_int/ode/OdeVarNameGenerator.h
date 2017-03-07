// OdeVarNameGenerator.h

#ifndef _ODE_ODEVARNAMEGENERATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ODE_ODEVARNAMEGENERATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "./interfaces/OdeRhs.h"
#include "../util/cxx_str_fmt_num.h"

#include <algorithm>
#include <cmath>

namespace ctm {
namespace math {

template< class VD >
class OdeVarNameGenerator
    {
    public:
        explicit OdeVarNameGenerator( const OdeRhs<VD>& rhs ) :
            m_rhs( rhs ),
            m_varIds( rhs.varIds() )
            {
            }

        std::string varName( unsigned int varId, unsigned int varIndex ) const
            {
            return formatUnnamedVar( varIndex );
            }

        unsigned int varIdByIndex( unsigned int varIndex ) const
            {
            ASSERT( varIndex < m_varIds.size() );
            return m_varIds[varIndex];
            }

        std::string varNameByIndex( unsigned int varIndex ) const {
            return varName( varIdByIndex( varIndex ), varIndex );
            }

    private:
        const OdeRhs<VD>& m_rhs;
        std::vector<unsigned int> m_varIds;

        static int ndigits( unsigned int n ) {
            return 1 + std::max( 0, static_cast<int>( log10(n + 0.5) ) );
            }

        std::string formatUnnamedVar( unsigned int i ) const
            {
            unsigned int pad = ndigits( m_rhs.varCount() ) - ndigits( i );
            return std::string("UNNAMED_") + std::string(pad, '0') + cxx::FormatInt( i );
            }
    };

} // end namespace math
} // end namespace ctm

#endif // _ODE_ODEVARNAMEGENERATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

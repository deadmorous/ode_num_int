// foreachindex.h

#ifndef _UTIL_FOREACHINDEX_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _UTIL_FOREACHINDEX_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

namespace ctm {

template< class Container, class F >
inline void foreachIndex( const Container& container, F f ) {
    unsigned int i = 0;
    for( const auto& item : container )
        f( item, i++ );
    }

} // end namespace ctm

#endif // _UTIL_FOREACHINDEX_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

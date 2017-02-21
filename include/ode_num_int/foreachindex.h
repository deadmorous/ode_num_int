// foreachindex.h

#ifndef _FOREACHINDEX_H_5F76209E_7B6E_4381_ABE7_E569BECE91BA_
#define _FOREACHINDEX_H_5F76209E_7B6E_4381_ABE7_E569BECE91BA_

namespace ctm {

template< class Container, class F >
inline void foreachIndex( const Container& container, F f ) {
    unsigned int i = 0;
    for( const auto& item : container )
        f( item, i++ );
    }

} // end namespace ctm

#endif // _FOREACHINDEX_H_5F76209E_7B6E_4381_ABE7_E569BECE91BA_

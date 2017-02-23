// per_type_storage.h

#ifndef _PER_TYPE_STORAGE_H_
#define _PER_TYPE_STORAGE_H_

#include <typeinfo>
#include <string>
#include <map>
#include "defs.h"

namespace ctm {

class PerTypeStorage
    {
    public:
        template< class ClassType, class ValueType >
        static ValueType& value() {
            return value< ValueType >( std::string(typeid(ClassType).name()) + " / " + typeid(ValueType).name() );
            }
    private:
        template< class ValueType >
        static ValueType& value( const std::string& key )
            {
            auto& d = data();
            auto it = d.find( key );
            if( it == d.end() ) {
                auto result = new ValueType;
                d[key] = result;
                return *result;
                }
            else
                return *reinterpret_cast<ValueType*>( it->second );
            }

        typedef std::map< std::string, void* > Data;

        static Data& data()
            {
            if( !m_data )
                m_data = new Data;
            return *m_data;
            }

        static ODE_NUMINT_API Data *m_data;
    };

} // end namespace ctm

#endif // _PER_TYPE_STORAGE_H_

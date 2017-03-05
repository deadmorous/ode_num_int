// factory.h

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>

#include "ctmstd/cxx_assert.h"
#include "ctmstd/cxx_exception.h"
#include "per_type_storage.h"

namespace ctm {

class FactoryBase
    {
    public:
        typedef std::string TypeId;
    };

template< class Interface_ >
class Factory : public FactoryBase
    {
    public:
        typedef Interface_ Interface;
        typedef std::shared_ptr< Interface > InterfacePtr;
        typedef std::function<InterfacePtr()> Generator;

        virtual ~Factory() {}

        static void registerType( const TypeId& typeId, Generator generator )
            {
            auto& r = registry();
            ASSERT( r.count(typeId) == 0 );
            r[typeId] = generator;
            }

        static InterfacePtr newInstance( const TypeId& typeId )
            {
            auto& r = registry();
            auto it = r.find( typeId );
            if( it == r.end() )
                throw cxx::exception( std::string("Failed to find type '") + typeId + "' in registry" );
            return it->second();
            }

        static std::vector< TypeId > registeredTypes()
            {
            auto& r = registry();
            std::vector< TypeId > result( r.size() );
            std::transform( r.begin(), r.end(), result.begin(), []( const typename Registry::value_type& v ) { return v.first; } );
            return result;
            }

        static bool isTypeRegistered( const TypeId& typeId ) {
            return registry().count( typeId ) > 0;
            }

    private:
        typedef std::map< TypeId, Generator > Registry;
        static Registry& registry() {
            return PerTypeStorage::value< Factory<Interface>, Registry >();
            }
    };

template< class Interface >
class TypeIdGetter
    {
    public:
        typedef typename Factory<Interface>::TypeId TypeId;

        virtual ~TypeIdGetter() {}
        virtual TypeId typeId() const = 0;

        template< class T >
        static TypeId typeId( const std::shared_ptr<T>& o ) {
            auto p = dynamic_cast< const TypeIdGetter<Interface>* >( o.get() );
            return p ?   p->typeId() :   TypeId();
            }
    };

template< class Type, class Interface >
class FactoryMixin : public TypeIdGetter< Interface >
    {
    public:
        static std::shared_ptr<Type> newInstance() {
            return std::make_shared<Type>();
            }
        class Registrator {
            public:
                explicit Registrator( typename Factory<Interface>::TypeId typeId ) {
                    Factory<Interface>::registerType( typeId, &FactoryMixin<Type, Interface>::newInstance );
                    PerTypeStorage::value< FactoryMixin<Type, Interface>, typename Factory<Interface>::TypeId >() = typeId;
                    }
            };
        static typename Factory<Interface>::TypeId staticTypeId() {
            return PerTypeStorage::value< FactoryMixin<Type, Interface>, typename Factory<Interface>::TypeId >();
            }

        typename Factory<Interface>::TypeId typeId() const {
            return staticTypeId();
            }
    };

template< class Implementation > struct ImplementationTypeTraits {
        static FactoryBase::TypeId typeId();
    };

template< class Implementation >
class ImplementationRegistrator : public Implementation::Registrator {
    public: ImplementationRegistrator() : Implementation::Registrator( ImplementationTypeTraits<Implementation>::typeId() ) {}
    };

} // end namespace ctm

#define CTM_FACTORY_REGISTER_TYPE( Class, typeId ) \
    static Class::Registrator Class##Registrator( typeId );

#define CTM_DECL_IMPLEMENTATION_TRAITS( Implementation, typeName ) \
template<> \
struct ImplementationTypeTraits< Implementation > { \
        static FactoryBase::TypeId typeId() { return typeName; } \
    };

#ifdef _MSC_VER
// Note: General implementation (below this one) causes C2910 in MSVS 2013
#define CTM_DECL_IMPLEMENTATION_TEMPLATE_TRAITS( Implementation, typeName ) \
template< class ... args > \
struct ImplementationTypeTraits< Implementation<args...> > { \
        static FactoryBase::TypeId typeId() { return typeName; } \
    };
#else // _MSC_VER
template<> template< class ... args > \
struct ImplementationTypeTraits< Implementation<args...> > { \
        static FactoryBase::TypeId typeId() { return typeName; } \
    };
#endif // _MSC_VER

#define CTM_DECL_IMPLEMENTATION_REGISTRATOR( Implementation ) \
    ImplementationRegistrator< Implementation > Implementation##Registrator;

#define CTM_DECL_IMPLEMENTATION_TEMPLATE_REGISTRATOR( Implementation, ... ) \
    ImplementationRegistrator< Implementation<__VA_ARGS__> > Implementation##Registrator;

#endif // _FACTORY_H_

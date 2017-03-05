// OptionalParameters.h

#ifndef _INFRA_OPTIONALPARAMETERS_H_CF3E_424f_9766_BA04D388199F_
#define _INFRA_OPTIONALPARAMETERS_H_CF3E_424f_9766_BA04D388199F_

#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <type_traits>

#include "ctmlex/lex_idnum.h"
#include "ctmstd/cxx_exception.h"
#include "ctmstd/cxx_str_fmt_num.h"
#include "Factory.h"

namespace ctm {

namespace OptionalParametersPrivate {

template< typename T > struct IsValidValueType : std::false_type {};

// template<> struct IsValidValueType<const char*> : std::true_type {};
template<> struct IsValidValueType<std::string> : std::true_type {};
template<> struct IsValidValueType<int> : std::true_type {};
template<> struct IsValidValueType<unsigned int> : std::true_type {};
template<> struct IsValidValueType<double> : std::true_type {};
template<> struct IsValidValueType<bool> : std::true_type {};
template<class Interface> struct IsValidValueType< std::shared_ptr<Interface> > : std::true_type {};

} // end namespace OptionalParametersPrivate

class OptionalParameters
    {
    public:
        typedef std::vector< std::string > Path;

        class Value;

        typedef std::map< std::string, Value > Parameters;

        class Value
            {
            public:
                Value() {}

                Value( const std::string& text, const Parameters& nestedParameters ) :
                    m_value( text ),
                    m_nestedParameters( std::make_shared< Parameters >( nestedParameters ) )
                    {}

                Value( const char* x ) : m_value( x ) {}
                Value( const std::string& x ) : m_value( x ) {}
                Value( int x ) : m_value( std::to_string( x ) ) {}
                Value( unsigned int x ) : m_value( std::to_string( x ) ) {}
                Value( double x )
                    {
                    std::ostringstream s;
                    s << std::setprecision(16) << x;
                    m_value = s.str();
                    }
                Value( bool x ) : m_value(x? "true": "false") {}

                template<class Interface>
                inline Value( const std::shared_ptr<Interface>& x );

                template< class T, typename = typename std::enable_if< OptionalParametersPrivate::IsValidValueType<T>::value >::type >
                T value() const {
                    T result;
                    toType( result );
                    return result;
                    }

                template< class T, typename = typename std::enable_if< OptionalParametersPrivate::IsValidValueType<T>::value >::type >
                operator T() const {
                    return value<T>();
                    }

                Parameters *nestedParameters() {
                    return m_nestedParameters.get();
                    }

                const Parameters *nestedParameters() const {
                    return m_nestedParameters.get();
                    }

                Parameters safeNestedParameters() const {
                    return m_nestedParameters ?   *m_nestedParameters :   Parameters();
                    }

            private:
                std::string m_value;
                std::shared_ptr<Parameters> m_nestedParameters;

                void toType( std::string& dst ) const {
                    dst = m_value;
                    }

                void toType( double& dst ) const
                    {
                    if( !lang::IsNumber( m_value.c_str() ) )
                        throw cxx::exception( std::string("Not a number: '") + m_value + "'");
                    dst = std::stod( m_value );
                    }

                void toType( int& dst ) const
                    {
                    if( !lang::IsIntegerNumber( m_value.c_str() ) )
                        throw cxx::exception( std::string("Not an integer number: '") + m_value + "'");
                    dst = std::stoi( m_value );
                    }

                void toType( unsigned int& dst ) const
                    {
                    if( !lang::IsIntegerNumber( m_value.c_str() ) )
                        throw cxx::exception( std::string("Not an integer number: '") + m_value + "'");
                    dst = static_cast<unsigned int>( std::stoul( m_value ) );
                    }

                void toType( bool& dst ) const
                    {
                    if( m_value == "0" || m_value == "false" )
                        dst = false;
                    else if( m_value == "1" || m_value == "true" )
                        dst = true;
                    else
                        throw cxx::exception( std::string("Not an boolean value: '") + m_value + "'");
                    }

                template<class Interface>
                inline void toType( std::shared_ptr<Interface>& dst ) const;
            };

        static Parameters& parameters( Parameters& root, const Path& path = Path() )
            {
            Parameters *result = &root;
            for( auto pathItem : path ) {
                auto it = result->find( pathItem );
                if( it == result->end() )
                    throw cxx::exception( std::string("Invalid path ") + formatPath(path) + ": item '" + pathItem + "' is not found" );
                auto nestedParameters = it->second.nestedParameters();
                if( !nestedParameters )
                    throw cxx::exception( std::string("Invalid path ") + formatPath(path) + ": item '" + pathItem + "' is a leaf" );
                result = nestedParameters;
                }
            return *result;
            }

        static Parameters& parameters( Parameters& root, const std::string& formattedPath ) {
            return parameters( root, parsePath( formattedPath ) );
            }

        static Value& value( Parameters& root, const Path& path )
            {
            Path parentPath;
            std::string name;
            std::tie( parentPath, name ) = decomposeValuePath( path );
            auto& params = parameters( root, parentPath );
            auto it = params.find( name );
            if( it == params.end() )
                throw cxx::exception( std::string("Invalid value path ") + formatPath(path) + ": value is not found" );
            return it->second;
            }

        static Value& value( Parameters& root, const std::string& formattedPath ) {
            return value( root, parsePath( formattedPath ) );
            }



        virtual ~OptionalParameters() {}

        virtual Parameters parameters() const {
            return Parameters();
            }

        virtual void setParameters( const Parameters& /* parameters */ ) {
            }

        virtual Parameters helpOnParameters() const {
            return Parameters();
            }

        virtual std::string helpOnType() const {
            return std::string();
            }

        template<class Interface>
        static Parameters defaultParameters( const std::string& typeId ) {
            return typeId.empty() ?   Parameters() :   Factory<Interface>::newInstance( typeId )->parameters();
            }

        template< class Setter >
        static bool maybeSetParameter( const Parameters& parameters, const std::string& name, Setter setter )
            {
            auto it = parameters.find( name );
            if( it == parameters.end() )
                return false;
            else {
                setter( it->second );
                return true;
                }
            }

        template< class T >
        static bool maybeLoadParameter( const Parameters& parameters, const std::string& name, T& dst )
            {
            return maybeSetParameter( parameters, name, [&dst]( T x ) {
                dst = x;
                } );
            }

        template< class Interface >
        static Value appendNestedHelp( const std::shared_ptr<Interface>& nested, const std::string& helpText )
            {
            Parameters nestedParameters;
            std::string helpOnType;
            if( auto op = dynamic_cast< const OptionalParameters* >( nested.get() ) ) {
                nestedParameters = op->helpOnParameters();
                helpOnType = op->helpOnType();
                }
            std::string typeId = TypeIdGetter<Interface>::typeId( nested );
            std::string text;
            if( !helpText.empty() )
                text += helpText + "\n\n";
            if( !helpOnType.empty() )
                text += "Information on current type(" + typeId + ")\n" + helpOnType + "\n\n";
            return Value( text + availableTypes<Interface>( typeId ), nestedParameters );
            }

    private:
        template< class Interface >
        static std::string availableTypes( const std::string& currentType )
            {
            std::string result;
            auto types = Factory<Interface>::registeredTypes();
            if( types.empty() )
                result += "There are no implementations available.";
            else {
                result += "The following implementations are available:";
                for( auto type : types ) {
                    result += type == currentType ?   "\n* " :   "\n  ";
                    result += type;
                    }
                }
            return result;
            }

        static std::tuple<Path, std::string> decomposeValuePath( const Path& valuePath )
            {
            if( valuePath.empty() )
                throw cxx::exception( "Value path is empty" );
            Path parentPath = valuePath;
            auto it = parentPath.end();
            --it;
            parentPath.erase( it );
            std::string name = valuePath.back();
            return std::make_tuple( parentPath, name );
            }

        static std::string formatPath( const Path& path )
            {
            std::string result;
            for( unsigned int i=0; i<path.size(); ++i ) {
                if( i > 0 )
                    result += ".";
                result += path[i];
                }
            return result;
            }

        static Path parsePath( const std::string& formattedPath )
            {
            Path result;
            auto n = 0u;
            while( n < formattedPath.size() ) {
                auto nd = formattedPath.find_first_of( '.', n );
                if( nd == std::string::npos )
                    nd = formattedPath.size();
                if( nd == n )
                    break;
                result.push_back( formattedPath.substr( n, nd-n ) );
                n = nd + 1;
                }
            return result;
            }
    };

template<class Interface>
inline OptionalParameters::Value::Value( const std::shared_ptr<Interface>& x ) {
    if( x ) {
        if( auto typeIdGetter = dynamic_cast< TypeIdGetter<Interface>* >(x.get()) )
            m_value = typeIdGetter->typeId();
        if( auto op = dynamic_cast< const OptionalParameters* >( x.get() ) )
            m_nestedParameters = std::make_shared<Parameters>( op->parameters() );
        }
    }

template<class Interface>
inline void OptionalParameters::Value::toType( std::shared_ptr<Interface>& dst ) const
    {
    if( m_value.empty() )
        dst = std::shared_ptr<Interface>();
    else {
        dst = Factory<Interface>::newInstance( m_value );
        if( m_nestedParameters ) {
            OptionalParameters *op = dynamic_cast< OptionalParameters* >( dst.get() );
            if( op )
                op->setParameters( *m_nestedParameters );
            }
        }
    }

inline std::ostream& operator<<( std::ostream& s, const OptionalParameters::Value& v ) {
    s << v.value<std::string>();
    return s;
    }

} // end namespace ctm

#endif // _INFRA_OPTIONALPARAMETERS_H_CF3E_424f_9766_BA04D388199F_

// OptionalParameters.h

/// \file
/// \brief Declaration of the OptionalParameters class and related types / functions.

#ifndef _INFRA_OPTIONALPARAMETERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_OPTIONALPARAMETERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <type_traits>

#include "./cxx_exception.h"
#include "./Factory.h"
#include "../util/lex_idnum.h"
#include "../util/cxx_str_fmt_num.h"

namespace ctm {

/// \brief Namespace used internally by the OptionalParameters class.
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

/// \brief Interface for a list of optional named parameters.
/// \note Trees of parameters can be organized because each parameter can be an object
/// supporting this interface.
class OptionalParameters
    {
    public:
        /// \brief Type for path identifying a node in a parameter tree.
        typedef std::vector< std::string > Path;

        class Value;

        /// \brief Type for a set of named parameters.
        typedef std::map< std::string, Value > Parameters;

        /// \brief Type holding a single value of any acceptable type, plus nested parameters.
        /// \note Value is stored as a string; nested parameters are stored as Parameters.
        class Value
            {
            public:
                /// \brief Default constructor. Value is initialized by an empty string; no nested parameters.
                Value() {}

                /// \brief Constructor.
                /// \param text Initializer for the field storing the value.
                /// \param nestedParameters Initializer for the field storing nested parameters.
                Value( const std::string& text, const Parameters& nestedParameters ) :
                    m_value( text ),
                    m_nestedParameters( std::make_shared< Parameters >( nestedParameters ) )
                    {}

                /// \brief Constructor accepting a C-string initializer (no nested parameters).
                Value( const char* x ) : m_value( x ) {}

                /// \brief Constructor accepting an std::string initializer (no nested parameters).
                Value( const std::string& x ) : m_value( x ) {}

                /// \brief Constructor accepting an integer initializer (no nested parameters).
                Value( int x ) : m_value( std::to_string( x ) ) {}

                /// \brief Constructor accepting an unsigned integer initializer (no nested parameters).
                Value( unsigned int x ) : m_value( std::to_string( x ) ) {}

                /// \brief Constructor accepting a double initializer (no nested parameters).
                Value( double x )
                    {
                    std::ostringstream s;
                    s << std::setprecision(16) << x;
                    m_value = s.str();
                    }

                /// \brief Constructor accepting a boolean initializer (no nested parameters).
                Value( bool x ) : m_value(x? "true": "false") {}

                /// \brief Constructor accepting an object initializer.
                /// \note The value is initialized by object type.
                /// \note Nested parameters
                /// are initialized by optional parameters of the object passed, if the object supports
                /// the OptionalParameters interface. Otherwise, there are no nested parameters.
                template<class Interface>
                inline Value( const std::shared_ptr<Interface>& x );

                /// \brief Returns value converted to type \a T.
                /// \note If \a T is a shared pointer to a factory interface, the string value is interpreted as
                /// a type identifier and passed to Factory<T>::newInstance(). Then nested parameters, if any,
                /// are set into the newly created instance by calling its OptionalParameters::safeNestedParameters()
                /// method.
                template< class T, typename = typename std::enable_if< OptionalParametersPrivate::IsValidValueType<T>::value >::type >
                T value() const {
                    T result;
                    toType( result );
                    return result;
                    }

                /// \brief Implicit cast to type \a T.
                template< class T, typename = typename std::enable_if< OptionalParametersPrivate::IsValidValueType<T>::value >::type >
                operator T() const {
                    return value<T>();
                    }

                /// \brief Returns pointer to nested parameters; if there are no nested parameters, returns null pointer.
                Parameters *nestedParameters() {
                    return m_nestedParameters.get();
                    }

                /// \brief Returns pointer to nested parameters (const overload); if there are no nested parameters, returns null pointer.
                const Parameters *nestedParameters() const {
                    return m_nestedParameters.get();
                    }

                /// \brief Returns a copy of nested parameters (empty if there are no nested parameters).
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

        /// \brief Returns parameters nested to the specified node of the specified set of parameters.
        /// \param root The set of parameters being queried.
        /// \param path Path identifying the node in \a root, whose nested parameters are requested.
        /// \return Reference to the set of parameters nested to node of \a root identified by \a path.
        /// \note If the path does not exist or is a leaf, an exception is thrown.
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

        /// \brief Returns parameters nested to the specified node of the specified set of parameters.
        /// \param root The set of parameters being queried.
        /// \param path Path identifying the node in \a root, whose nested parameters are requested.
        /// Path elements must be separated by dot.
        /// \return Reference to the set of parameters nested to node of \a root identified by \a path.
        /// \note If the path does not exist or is a leaf, an exception is thrown.
        static Parameters& parameters( Parameters& root, const std::string& formattedPath ) {
            return parameters( root, parsePath( formattedPath ) );
            }

        /// \brief Returns value at specified path of the specified set of parameters.
        /// \param root The set of parameters being queried.
        /// \param path Path identifying the node in \a root, whose value is requested.
        /// \return Reference to the value of the node identified by \a path in \a root.
        /// \note If the path does not exist, an exception is thrown.
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

        /// \brief Returns value at specified path of the specified set of parameters.
        /// \param root The set of parameters being queried.
        /// \param path Path identifying the node in \a root, whose value is requested.
        /// Path elements must be separated by dot.
        /// \return Reference to the value of the node identified by \a path in \a root.
        /// \note If the path does not exist, an exception is thrown.
        static Value& value( Parameters& root, const std::string& formattedPath ) {
            return value( root, parsePath( formattedPath ) );
            }

        /// \brief Virtual destructor.
        virtual ~OptionalParameters() {}

        /// \brief Returns parameters of this instance.
        virtual Parameters parameters() const {
            return Parameters();
            }

        /// \brief Sets parameters of this instance.
        virtual void setParameters( const Parameters& /* parameters */ ) {
            }

        /// \brief Returns help strings on parameters of this instance.
        virtual Parameters helpOnParameters() const {
            return Parameters();
            }

        /// \brief Returns help string on this type.
        virtual std::string helpOnType() const {
            return std::string();
            }

        /// \brief Returns parameters of default-constructed instance of the specified type.
        /// \param Interface Factory interface (used to create a fresh new instance for getting its parameters).
        /// \param typeId Type identifier of object whose default parameters are requested.
        /// \return Parameters of default-constructed instance type \a typeId created by Factory<Interface>.
        template<class Interface>
        static Parameters defaultParameters( const std::string& typeId ) {
            return typeId.empty() ?   Parameters() :   Factory<Interface>::newInstance( typeId )->parameters();
            }

        /// \brief Sets parameter if it exists, using a setter function.
        /// \param parameters Set of parameters where the parameter to be set is beeing looked up.
        /// \param name Parameter name
        /// \param setter Function called to set parameter into this object.
        /// \return True if parameter is found, false otherwise.
        /// \note If the parameter with name \a name is not found in \a parameters,
        /// nothing is done and \a false is returned.
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

        /// \brief Sets parameter if it exists, using the reference to the storage.
        /// \param parameters Set of parameters where the parameter to be set is beeing looked up.
        /// \param name Parameter name
        /// \param dst Reference to the value where the parameter is to be stored.
        /// \return True if parameter is found, false otherwise.
        /// \note If the parameter with name \a name is not found in \a parameters,
        /// nothing is done and \a false is returned.
        template< class T >
        static bool maybeLoadParameter( const Parameters& parameters, const std::string& name, T& dst )
            {
            return maybeSetParameter( parameters, name, [&dst]( T x ) {
                dst = x;
                } );
            }

        /// \brief Appends help text for nested parameters of an object.
        /// \param Interface Factory interface.
        /// \param nested Object whose help is to be requested.
        /// \param helpText Help text corresponding to a parameter whose value is currently \a nested.
        /// \return Value whose value is \a helpText, followed by the list of available implementations of \a Interface,
        /// and whose nested values are what helpOnParameters() returns for \a nested, if it supports
        /// the OptionalParameters interface.
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

/// \brief Performs formatted output of a value to a stream.
/// \param s stream where the output is written to.
/// \param v value to output.
/// \return \a s.
/// \note Nested parameters of the value are not written to the stream.
inline std::ostream& operator<<( std::ostream& s, const OptionalParameters::Value& v ) {
    s << v.value<std::string>();
    return s;
    }

} // end namespace ctm

#endif // _INFRA_OPTIONALPARAMETERS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

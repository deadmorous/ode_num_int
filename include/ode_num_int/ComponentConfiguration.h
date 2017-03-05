// ComponentConfiguration.h

#ifndef _COMPONENTCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _COMPONENTCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "opt_param.h"
#include <set>

namespace ctm {

class ComponentConfiguration
    {
    public:
        typedef OptionalParameters::Path Path;

        ComponentConfiguration() :
            m_parametersKnown( true )
            {}

        explicit ComponentConfiguration( std::function< std::shared_ptr<OptionalParameters>() > parameterProviderSupplier ) :
            m_parameterProviderSupplier( parameterProviderSupplier ),
            m_parametersKnown( false )
            {}

        void setParameterProvider( const std::shared_ptr<OptionalParameters>& parameterProvider )
            {
            m_parameterProvider = parameterProvider;
            m_parametersKnown = false;
            }

        std::shared_ptr<OptionalParameters> parameterProvider() const
            {
            if( !m_parameterProvider )
                m_parameterProvider = m_parameterProviderSupplier();
            return m_parameterProvider;
            }

        const OptionalParameters::Parameters& parameters( const Path& path = Path() ) const {
            return parametersPriv( path );
            }

        const OptionalParameters::Parameters& parameters( const std::string& formattedPath ) const {
            return parametersPriv( formattedPath );
            }

        const OptionalParameters::Value& value( const Path& path ) const {
            return valuePriv( path );
            }

        const OptionalParameters::Value& value( const std::string& formattedPath ) const {
            return valuePriv( formattedPath );
            }

        void setValue( const Path& path, const OptionalParameters::Value& value ) const {
            setValuePriv( path, value );
            }

        void setValue( const std::string& formattedPath, const OptionalParameters::Value& value ) const {
            setValuePriv( formattedPath, value );
            }

        std::string helpOnValue( const Path& path ) const {
            return helpOnValuePriv( path );
            }

        std::string helpOnValue( const std::string& formattedPath ) const {
            return helpOnValuePriv( formattedPath );
            }

    private:
        mutable std::shared_ptr<OptionalParameters> m_parameterProvider;
        std::function< std::shared_ptr<OptionalParameters>() > m_parameterProviderSupplier;
        mutable OptionalParameters::Parameters m_parameters;
        mutable OptionalParameters::Parameters m_helpOnParameters;
        mutable bool m_parametersKnown;

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

        template< class PathType >
        OptionalParameters::Parameters& parametersPriv( const PathType& path ) const
            {
            if( !m_parametersKnown ) {
                auto pp = parameterProvider();
                if( pp ) {
                    m_parameters = pp->parameters();
                    m_helpOnParameters = pp->helpOnParameters();
                    m_parametersKnown = true;
                    }
                }
            return OptionalParameters::parameters( m_parameters, path );
            }

        template< class PathType >
        OptionalParameters::Value& valuePriv( const PathType& path ) const {
            parametersPriv( Path() );   // Make sure parameters are loaded
            return OptionalParameters::value( m_parameters, path );
            }

        template< class PathType >
        void setValuePriv( const PathType& path, const OptionalParameters::Value& value ) const
            {
            valuePriv( path ) = value;

            // Always reload auto setting, in particular, to load nested parameters
            parameterProvider()->setParameters( m_parameters );
            m_parametersKnown = false;
            }

        template< class PathType >
        std::string helpOnValuePriv( const PathType& path ) const {
            parametersPriv( Path() );   // Make sure parameters are loaded
            std::string help;
            try {
                help = OptionalParameters::value( m_helpOnParameters, path );
                }
            catch( std::exception& ) {}
            return help.empty() ?   std::string("No help is available"):   help;
            }

    };

} // end namespace ctm

#endif // _COMPONENTCONFIGURATION_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

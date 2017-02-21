// ode_solver.cpp

#include "ode_solver.h"
#include "vector_for_ode_solver.h"
#include "id_name_mapper.h"
#include "foreachindex.h"

#include <fstream>
#include <sstream>
#include <set>

namespace ctm {
namespace math {


void ScaledErrorCalculatorUtil::loadScalingPriv(
        Vector<double>& dst,
        const std::string& fileName,
        const std::vector<unsigned int>& ids )
    {
    using namespace std;

    // Allocate memory
    auto n = ids.size();
    dst.resize( n );

    // Open input file
    ifstream is( fileName );
    if( !is.is_open() )
        throw cxx::exception( string("Failed to open file '") + fileName + "'" );

    // Load id -> error mapping
    map< unsigned int, double > scaleMapping;
    IdNameMapper idnm;
    for( unsigned int iline=0; ; ++iline ) {
        auto err = [&]( const string& msg ) {
            ostringstream oss;
            oss << "Failed to read file '" << fileName << "', line " << (iline+1) << ":\n"
                << msg;
            throw cxx::exception( oss.str() );
            };
        string line;
        getline( is, line );
        if( is.fail() )
            break;
        if( line.empty() )
            continue;
        auto pos = line.find_first_of( '\t' );
        if( pos == string::npos )
            err( "Missing TAB character" );
        auto idName = line.substr( 0, pos );
        unsigned int id;
        try {
            id = idnm.valByName( idName );
            }
        catch( const std::exception& e ) {
            err( e.what() );
            }
        if( scaleMapping.count( id ) != 0 )
            err( string("Id '") + idName + "' is specified more than once" );

        auto valueText = line.substr( pos+1 );
        if( !lang::IsNumber( valueText.c_str() ) )
            err( "A number after TAB was expected" );
        auto value = std::stod( valueText );
        if( value <= 0 )
            err( "The number after TAB must be positive" );

        scaleMapping[id] = 1. / value;
        }

    set<unsigned int> unknownIds;
    foreachIndex( ids, [&]( unsigned int id, unsigned int i ) {
        auto it = scaleMapping.find( id );
        double s = 1;
        if( it == scaleMapping.end() )
            unknownIds.insert( id );
        else
            s = it->second;
        dst[i] = s;
        } );
    if( !unknownIds.empty() ) {
        ostringstream oss;
        oss << "Some IDs are not specified:\n";
        bool first = false;
        for( auto id: unknownIds ) {
            if( first )
                first = false;
            else
                oss << ", ";
            try {
                oss << idnm.valName( id );
                }
            catch( std::exception& ) {
                oss << id;
                }
            }
        throw cxx::exception( oss.str() );
        }
    }

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( ErrorInfNormCalculator, "inf" )
CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( ScaledErrorCalculator, "scaled" )

CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( OdeStepSizeSimpleController, "simple" )

} // end namespace math
} // end namespace ctm

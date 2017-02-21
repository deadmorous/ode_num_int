// id_name_mapper.h

#ifndef _ID_NAME_MAPPER_H_
#define _ID_NAME_MAPPER_H_

#include "translid/translid.h"
#include "sim_dyn_util/sim_dyn_util.h"
#include "ctmstd/cxx_str_fmt_num.h"

namespace ctm {

class IdNameMapper
    {
    public:
        std::string name( unsigned int id, gui::ID_KIND idKind = gui::IDK_NONE ) const {
            return nameMapper()->GetIdName( id, idKind );
            }
        std::string oidName( unsigned int id ) const {
            return name( id, gui::IDK_OID );
            }
        std::string valName( unsigned int id ) const {
            return name( id, gui::IDK_VAL );
            }
        std::string dofName( unsigned int id, unsigned int order = 0 ) const
            {
            auto translatedDofId = mech::TranslateDofId( id, order );
            try {
                return name( translatedDofId, gui::IDK_VAL );
                }
            catch( std::exception& ) {
                return cxx::FormatHex( translatedDofId );
                }
            }
        unsigned int byName( const std::string& name, gui::ID_KIND idKind = gui::IDK_NONE ) const {
            return nameMapper()->GetIdByName( name, idKind );
            }
        unsigned int oidByName( const std::string& name ) const {
            return byName( name, gui::IDK_OID );
            }
        unsigned int valByName( const std::string& name ) const {
            return byName( name, gui::IDK_VAL );
            }
    private:
        mutable storage::WBIIdNameMapper m_nameMapper;
        storage::WBIIdNameMapper nameMapper() const
            {
            using namespace storage;
            using namespace inproc::om;
            if( !m_nameMapper ) {
                WSIIdTranslatorFactory factory = ObjectFactory::CreateCompoundObject( translid::OID_ID_TRANSLATOR_FACTORY );
                ASSERT( factory );
                m_nameMapper = factory->GetIdNameMapper();
                ASSERT( m_nameMapper );
                }
            return m_nameMapper;
            }
    };

} // end namespace ctm

#endif // _ID_NAME_MAPPER_H_

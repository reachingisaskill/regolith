
#ifndef REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_
#define REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/FactoryBuilder.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <map>
#include <string>


namespace Regolith
{

  template< class TYPE, class ... ARGS >
  class FactoryTemplate
  {
    typedef std::map< std::string, FactoryBuilderBase< TYPE, ARGS... >* > BuilderMap;

    private:
      BuilderMap _builders;

    public:
      FactoryTemplate() : _builders() {}

      virtual ~FactoryTemplate();

      template < class DERIVED >
      void addBuilder( std::string );

      virtual TYPE* build( Json::Value&, ARGS... ) const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template Member Functions


  template < class TYPE, class ... ARGS >
  FactoryTemplate<TYPE, ARGS...>::~FactoryTemplate()
  {
    for ( typename BuilderMap::iterator it = _builders.begin(); it != _builders.end(); ++it )
    {
      delete it->second;
    }
    _builders.clear();
  }


  template< class TYPE, class ... ARGS >
  TYPE* FactoryTemplate<TYPE, ARGS...>::build( Json::Value& data, ARGS... args ) const
  {
    validateJson( data, "type", JsonType::STRING, true );

    std::string name = data["type"].asString();

    typename BuilderMap::const_iterator found = _builders.find( name );

    if ( found == _builders.end() )
    {
      Exception ex( "FactoryTemplate<>::build()", "Could not find requested builder" );
      ex.addDetail( "Type name", name );
      throw ex;
    }

    return found->second->create( data, args... );
  }


  template < class TYPE, class ... ARGS > template < class DERIVED >
  void FactoryTemplate<TYPE, ARGS...>::addBuilder( std::string type_name )
  {
    typename BuilderMap::iterator found = _builders.find( type_name );
    if ( found != _builders.end() )
    {
      WARN_STREAM << "A builder already exists with type name: " << type_name << ". It will be replaced.";
      _builders.erase( found );
    }
    
    _builders[ type_name ] = new FactoryBuilder< TYPE, DERIVED, ARGS... >();
  }

}

#endif // REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_



#ifndef REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_
#define REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/MassProduceable.h"
#include "Regolith/Architecture/FactoryBuilder.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <map>
#include <string>


namespace Regolith
{

  template< class TYPE >
  class FactoryTemplate
  {
    typedef std::map< std::string, FactoryBuilderBase< TYPE >* > BuilderMap;

    private:
      BuilderMap _builders;

    public:
      FactoryTemplate() : _builders() {}

      virtual ~FactoryTemplate();

      template < class DERIVED >
      void addBuilder( std::string );

      virtual TYPE* build( Json::Value& ) const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template Member Functions


  template < class TYPE >
  FactoryTemplate<TYPE>::~FactoryTemplate()
  {
    for ( typename BuilderMap::iterator it = _builders.begin(); it != _builders.end(); ++it )
    {
      delete it->second;
    }
    _builders.clear();
  }


  template< class TYPE >
  TYPE* FactoryTemplate<TYPE>::build( Json::Value& data ) const
  {
    Utilities::validateJson( data, "type", Utilities::JSON_TYPE_STRING, true );

    std::string name = data["type"].asString();

    typename BuilderMap::const_iterator found = _builders.find( name );

    if ( found == _builders.end() )
    {
      FAILURE_STREAM << "Could not find requested builder for type: " << name;
      Exception ex( "FactoryTemplate<>::build()", "Could not find requested builder" );
      ex.addDetail( "Type name", name );
      throw ex;
    }

    return found->second->create( data );
  }


  template < class TYPE > template < class DERIVED >
  void FactoryTemplate<TYPE>::addBuilder( std::string type_name )
  {
    typename BuilderMap::iterator found = _builders.find( type_name );
    if ( found != _builders.end() )
    {
      WARN_STREAM << "A builder already exists with type name: " << type_name << ". It will be replaced.";
      _builders.erase( found );
    }
    
    _builders[ type_name ] = new FactoryBuilder< TYPE, DERIVED >();
  }

}

#endif // REGOLITH_ARCHITECTURE_FACTORY_TEMPLATE_H_


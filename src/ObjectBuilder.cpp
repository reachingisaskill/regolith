
#include "ObjectBuilder.h"

#include "Exception.h"
#include "Drawable.h"
#include "Sprite.h"
#include "Manager.h"

#include "logtastic.h"


namespace Regolith
{

  ObjectBuilder::ObjectBuilder() :
    _factories(),
    _theRenderer()
  {
  }


  ObjectBuilder::ObjectBuilder( SDL_Renderer* rend ) :
    _factories(),
    _theRenderer( rend )
  {
  }


  ObjectBuilder::~ObjectBuilder()
  {
    // Delete the individual factories
    for ( FactoryMap::iterator it = _factories.begin(); it != _factories.end(); ++it )
    {
      delete it->second;
    }
    _factories.clear();
  }


  void ObjectBuilder::setRenderer( SDL_Renderer* rend )
  {
    _theRenderer = rend;
  }


  void ObjectBuilder::setScene( const Scene* scene )
  {
    _theScene = scene;
  }


  void ObjectBuilder::addFactory( ObjectFactory_base* factory )
  {
    std::string name( factory->getObjectName() );

    if ( _factories.find( name ) != _factories.end() )
    {
      ERROR_LOG( "Attempting to add factory with a name that is already loaded" );
      return;
    }

    factory->_parentBuilder = this;

    _factories[name] = factory;
  }


  Drawable* ObjectBuilder::build( Json::Value jsonObject )
  {
    std::string object_type = jsonObject["resource_type"].asString();
    INFO_STREAM << "Building texture of type : " << object_type;

    FactoryMap::iterator found = _factories.find( object_type );

    if ( found == _factories.end() )
    {
      ERROR_STREAM << "Object type not found in factory list: " << object_type;
      Exception ex( "ObjectBuilder::build()", "Non-implemented build type", true );
      ex.addDetail( "TypeID", object_type );
      throw ex;
    }

    return found->second->build( jsonObject );
  }

}


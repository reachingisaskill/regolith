
#include "SceneBuilder.h"

#include "Utilities.h"
#include "Exception.h"
#include "Scene.h"

#include "logtastic.h"


namespace Regolith
{

  SceneBuilder::SceneBuilder() :
    _factories()
  {
  }


  SceneBuilder::~SceneBuilder()
  {
    for ( SceneFactoryMap::iterator it = _factories.begin(); it != _factories.end(); ++it )
    {
      delete it->second;
    }
    _factories.clear();
  }


  void SceneBuilder::addFactory( SceneFactory_base* factory )
  {
    std::string name( factory->getSceneName() );

    if ( _factories.find( name ) != _factories.end() )
    {
      ERROR_LOG( "Attempting to add factory with a name that is already loaded" );
      return;
    }

    _factories[ name ] = factory;
  }


  Scene* SceneBuilder::build( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "type", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "file_name", Utilities::JSON_TYPE_STRING );

    std::string filename = json_data["file_name"].asString();
    std::string type = json_data["type"].asString();

    INFO_STREAM << "Building Scene of type : " << type;

    SceneFactoryMap::iterator found = _factories.find( type );

    if ( found == _factories.end() )
    {
      ERROR_STREAM << "Object type not found in factory list: " << type;
      Exception ex( "ObjectBuilder::build()", "Non-implemented build type", true );
      ex.addDetail( "TypeID", type );
      throw ex;
    }

    Scene* the_scene =  found->second->build( filename );
    return the_scene;
  }

}


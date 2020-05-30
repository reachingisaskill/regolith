
#include "Regolith/Managers/DataHandler.h"


namespace Regolith
{
  DataHandler::DataHandler( NamedVector<GameObject, true >& objects ) :
    _requiredTextures(),
    _gameObjects( objects ),
    _loadScreen( 0 )
  {
  }


  DataHandler::~DataHandler()
  {
    INFO_LOG( "Destroying Data Handler" );

    INFO_LOG( "Deleting GameObjects" );
    _gameObjects.clear();
  }


  void DataHandler::load( DataManager::DataBuilder& builder )
  {
    std::vector<std::string>::iterator end = _requiredTextures.end();
    for ( std::vector<std::string>::iterator it = _requiredTextures.begin(); it != end; ++it )
    {
      builder.loadTexture( (*it) );
    }
  }


  void DataHandler::unload( DataManager::DataBuilder& builder )
  {
    std::vector<std::string>::iterator end = _requiredTextures.end();
    for ( std::vector<std::string>::iterator it = _requiredTextures.begin(); it != end; ++it )
    {
      builder.unloadTexture( (*it) );
    }
  }


  RawTexture* findTexture( std::string name )
  {
    // Find the id number
    unsigned int id = Manager::getInstance()->DataManager().requestTexture( name );
    
    if ( ! Manager::getInstance()->DataManager().isGlobal( id ) )
    {
      // Add it to the current handler ONLY if the global handler doesnt already own it
      _requiredTextures.push_back( id );
    }

    // Return the requested texture pointer
    return Manager::getInstance()->DataManager().getRawTexture( id );
  }

}


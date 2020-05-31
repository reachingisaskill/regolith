
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Utilities/NamedVector.h"
#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{
  DataHandler::DataHandler() :
    _handlerID( 0 ),
    _requiredTextures(),
    _gameObjects( Manager::getInstance()->getDataManager().gameObjects() ),
    _loadScreen( 0 )
  {
  }


  DataHandler::~DataHandler()
  {
    INFO_LOG( "Destroying Data Handler" );
  }


  void DataHandler::configure( std::string name )
  {
    Manager::getInstance()->getDataManager().configureHandler( this, name );
  }


  RawTexture* DataHandler::findTexture( std::string name )
  {
    // Find the id number
    unsigned int id = Manager::getInstance()->getDataManager().requestRawTexture( name );
    
    if ( ! Manager::getInstance()->getDataManager().isGlobal( id ) )
    {
      // Add it to the current handler ONLY if the global handler doesnt already own it
      _requiredTextures->push_back( id );
    }

    // Return the requested texture pointer
    return Manager::getInstance()->getDataManager().getRawTexture( id );
  }

}


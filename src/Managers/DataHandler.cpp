
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataManager.h"


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


//  void DataHandler::configure( std::string name )
//  {
//    Manager::getInstance()->getDataManager().configureHandler( this, name );
//  }


  RawTexture* DataHandler::findTexture( std::string name )
  {
    // Find the id number
    RawTexturePointer pointer = Manager::getInstance()->getDataManager().requestRawTexture( name );

    DEBUG_STREAM << "Finding texture: " << name << " - " << (Manager::getInstance()->getDataManager().isGlobal( pointer )? " is " : " is not " ) << "global";
    if ( ! Manager::getInstance()->getDataManager().isGlobal( pointer ) )
    {
      // Add it to the current handler ONLY if the global handler doesnt already own it
      _requiredTextures->insert( pointer );
    }

    // Return the requested texture pointer
    RawTexture* temp = Manager::getInstance()->getDataManager().getRawTexture( pointer->first );
    DEBUG_STREAM << "Found : " << temp;
    return temp;
  }


  bool DataHandler::isCached( RawTexturePointer p ) const
  {
    if ( _requiredTextures->find( p ) == _requiredTextures->end() )
      return false;
    else
      return true;
  }

}


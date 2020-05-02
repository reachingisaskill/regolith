
#include "SceneTitle.h"

#include "Exception.h"
#include "Utilities.h"
#include "Manager.h"
#include "Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  SceneTitle::SceneTitle( std::string json_file ) :
    Scene( json_file )
  {
  }


  SceneTitle::~SceneTitle()
  {
    INFO_LOG( "Destroying Scene - Title" );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // All the loading functions to import Json data and build the necessary objects


  void SceneTitle::_loadSceneSpecificComponents( Json::Value& json_data )
  {
  }


  void SceneTitle::_loadOptions( Json::Value& options )
  {
    Utilities::validateJson( options, "starting_dialog", Utilities::JSON_TYPE_STRING, true );

    std::string dialog_name = options["starting_dialog"].asString();
    _startingDialog = dialogWindows().getByName( dialog_name );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // SceneTitle update and render functions

  void SceneTitle::_render()
  {
  }


  void SceneTitle::_update( Uint32 )
  {
  }


  void SceneTitle::_resolveCollisions()
  {
  }


  void SceneTitle::raiseContextEvent( ContextEvent context_event )
  {
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Hooks

  void SceneTitle::onStart()
  {
    DEBUG_LOG( "On start called" );
    if ( defaultMusic() >= 0 )
    {
      audioHandler()->setSong( defaultMusic() );
    }

    _startingDialog->giveFocus();

  }


  void SceneTitle::returnFocus()
  {
    Manager::getInstance()->raiseEvent( REGOLITH_EVENT_QUIT );
  }

}


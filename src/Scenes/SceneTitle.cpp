
#include "Scenes/SceneTitle.h"
#include "Managers/Manager.h"
#include "Components/Exception.h"
#include "Components/Utilities.h"
#include "Components/Collision.h"

#include "logtastic.h"

#include <fstream>


namespace Regolith
{


  SceneTitle::SceneTitle( std::string h ) :
    Scene( h ),
    _startingDialog( nullptr )
  {
  }


  SceneTitle::~SceneTitle()
  {
    INFO_LOG( "Destroying Scene - Title" );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // SceneTitle update and render functions

  void SceneTitle::_render()
  {
    DEBUG_LOG( "Rendering Title" );
  }


  void SceneTitle::_update( Uint32 )
  {
  }


  void SceneTitle::_resolveCollisions()
  {
  }


  void SceneTitle::raiseContextEvent( ContextEvent )
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

    DEBUG_STREAM << "Opening starting dialog: " << _startingDialog;
    Manager::getInstance()->openContext( _startingDialog );
  }


  void SceneTitle::returnFocus()
  {
//    Manager::getInstance()->raiseEvent( REGOLITH_EVENT_QUIT );
  }

}


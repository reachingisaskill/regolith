
#include "Regolith/Test/LoadScreen.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  LoadScreen::LoadScreen() :
    Context()
  {
    DEBUG_LOG( "LoadScreen::LoadScreen : LoadScreen Created" );
  }


  LoadScreen::~LoadScreen()
  {
    DEBUG_LOG( "LoadScreen::~LoadScreen : LoadScreen Destroyed" );
  }


  void LoadScreen::onStart()
  {
    this->setClosed( false );
  }


  void LoadScreen::updateContext( float )
  {
  }

}


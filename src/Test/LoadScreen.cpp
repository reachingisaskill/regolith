
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
    static Manager* manager = Manager::getInstance();

//    if ( manager->getContextManager().isLoaded() )
//    {
//      DEBUG_LOG( "LoadScreen::updateContext : Load screen complete." );
//      manager->openEntryPoint();
//      this->stopContext();
//    }
  }

}



#include "Context.h"
#include "Manager.h"


namespace Regolith
{

  Context::Context( InputHandler* h ) :
    _theInput( h ),
    _theAudio( Manager::getInstance()->getAudioManager() )
  {
  }


  void Context::giveFocus()
  {
    Manager::getInstance()->pushContext( this );
    _theAudio.play();
  }


  void Context::transferFocus( Context* newContext )
  {
    _theAudio.pause();
    newContext->giveFocus();
  }


  void Context::takeFocus()
  {
    _theAudio.stop();
    Manager::getInstance()->popContext();
  }


  void Context::returnFocus()
  {
    _theAudio.play();
  }

}


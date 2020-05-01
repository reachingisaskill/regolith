
#include "Context.h"
#include "Manager.h"


namespace Regolith
{

  Context::Context() :
    _parent( nullptr ),
    _theInput(),
    _theAudio( Manager::getInstance()->getAudioManager() ),
    _properties( CONTEXT_PROPERTIES_NONE )
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



#include "Dialog.h"
#include "Manager.h"


namespace Regolith
{

  // Dialog Window

  DialogWindow::DialogWindow( Scene* scene, Camera* camera, std::string file ) :
    _filename( file ),
    _scene( scene ),
    _camera( camera ),
    _dialogTree( nullptr ),
    _currentDialog( nullptr )
  {
  }


  void DialogWindow::loadFromJson()
  {
  }


  void DialogWindow::update( Uint32 time )
  {
    _currentDialog->update( time );
  }


  void DialogWindow::render()
  {
    _currentDialog->render();
  }


  void DialogWindow::raiseContextEvent( ContextEvent context_event )
  {
    static Manager* man = Manager::getInstance();

    switch ( context_event )
    {
      case CONTEXT_EVENT_QUIT :
        man->raiseEvent( REGOLITH_QUIT );
        break;

      case CONTEXT_EVENT_NEXT :

        break;
        
      case CONTEXT_EVENT_PREV :

        break;

      case CONTEXT_END :
        getParentContext()->raiseContextEvent( CONTEXT_FINISHED );
        break;

      case CONTEXT_EVENT_DEATH :
      case CONTEXT_EVENT_RESPAWN :
      case CONTEXT_EVENT_SKIP :

      default:
        if ( (unsigned)context_event > (unsigned)CONTEXT_EVENT_OPTIONS )
        {
          _currentDialog = _currentDialog->selectDialog( (unsigned)context_event - (unsigned)CONTEXT_EVENT_OPTIONS );
        }
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Class

  Dialog::Dialog( Scene* scene, Camera* camera, Json::Value& json_data ) :
    _owner( scene ),
    _HUDCamera( camera ),
    _subDialogs("sub_dialogs"),
    _elements(),
    _background(),
    _buttons()
  {
  }


  void Dialog::update( Uint32 time )
  {
  }


  void Dialog::render()
  {
  }


  Dialog* Dialog::selectDialog( unsigned int n )
  {
    return _subDialogs[ n ];
  }

}


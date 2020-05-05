
#include "Dialog.h"
#include "Manager.h"
#include "ObjectBuilder.h"
#include "Utilities.h"
#include "Signal.h"


namespace Regolith
{

  Dialog::Dialog( Camera* camera, std::string h ) :
    Context( h ),
    _cameraHUD( camera ),
    _elements(),
    _background(),
    _buttons()
  {
  }


  Dialog::~Dialog()
  {
    // Clear the button cache
    _buttons.clear();

    // Clear the caches
    _animated.clear();

    // Delete the background
    delete _background;
    _background = nullptr;

    // Delete all the elements
    ElementList::iterator end = _animated.end();
    for ( ElementList::iterator it = _animated.begin(); it != end; ++it )
    {
      delete (*it);
    }
    _elements.clear();
  }


  void Dialog::update( Uint32 time )
  {
    // Render each element in the element list
    ElementList::iterator end = _animated.end();
    for ( ElementList::iterator it = _animated.begin(); it != end; ++it )
    {
      (*it)->update( time );
    }

    this->_update( time );
  }


  void Dialog::render()
  {
    DEBUG_LOG( "Rendering Dialog" );
    // First draw the background
    if ( _background != nullptr )
      _background->render( _cameraHUD );

    // Render each element in the element list
    DEBUG_STREAM << "Rendering " << _elements.size() << " Dialog elements";
    ElementList::iterator end = _elements.end();
    for ( ElementList::iterator it = _elements.begin(); it != end; ++it )
    {
      (*it)->render( _cameraHUD );
    }

    this->_render( _cameraHUD );
  }


  void Dialog::registerActions( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_QUIT );
    handler->registerInputRequest( this, INPUT_ACTION_NEXT );
    handler->registerInputRequest( this, INPUT_ACTION_PREV );
    handler->registerInputRequest( this, INPUT_ACTION_SELECT );
    handler->registerInputRequest( this, INPUT_ACTION_BACK );
    handler->registerInputRequest( this, INPUT_ACTION_CANCEL );
    handler->registerInputRequest( this, INPUT_ACTION_MOUSE_MOVE );
    handler->registerInputRequest( this, INPUT_ACTION_CLICK ); // Bind the mouse click
  }


  void Dialog::booleanAction( const InputAction& action, bool value )
  {
    DEBUG_STREAM << "BOOLEAN ACTION : " << action;
    switch ( action )
    {
      case INPUT_ACTION_QUIT :
        if ( value ) Manager::getInstance()->raiseEvent( REGOLITH_EVENT_QUIT );
        break;

      case INPUT_ACTION_NEXT :
        if ( value ) _buttons.focusNext();
        break;

      case INPUT_ACTION_PREV :
        if ( value ) _buttons.focusPrev();
        break;

      case INPUT_ACTION_SELECT :
      case INPUT_ACTION_JUMP :
        if ( value ) _buttons.select();
        break;

      case INPUT_ACTION_PAUSE :
      case INPUT_ACTION_BACK :
      case INPUT_ACTION_CANCEL :
        if ( _canCancel && value ) Manager::getInstance()->closeContext();
        break;

      default :
        break;
    }
  }


  void Dialog::vectorAction( const InputAction& action, const Vector& vec )
  {
    DEBUG_STREAM << "VECTOR ACTION : " << action;
    switch ( action )
    {
      case INPUT_ACTION_MOUSE_MOVE :
        _buttons.mouseMove( vec );
        break;

      default:
        break;
    }
  }


  void Dialog::mouseAction( const InputAction& action, bool click, const Vector& vec )
  {
    DEBUG_STREAM << "MOUSE ACTION : " << action;
    switch ( action )
    {
      case INPUT_ACTION_CLICK :
        if ( click )
        {
          _buttons.mouseDown( vec );
        }
        else
        {
          _buttons.mouseUp( vec );
        }
        break;

      default:
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Menu Dialog

  MenuDialog::MenuDialog( Camera* camera, std::string inputMap ) :
    Dialog( camera, inputMap ),
    _subDialogs("sub_dialogs")
  {
  }

  
  MenuDialog::~MenuDialog()
  {
    // Delete the subDialogs
    _subDialogs.clear();
  }


  Dialog* MenuDialog::selectDialog( unsigned int n )
  {
    return _subDialogs[ n ];
  }


  void MenuDialog::raiseContextEvent( ContextEvent event )
  {
    switch ( event )
    {
      default :
        int dialogNum = (int)event - (int)CONTEXT_EVENT_OPTIONS;
        if ( dialogNum >= 0 )
        {
          Manager::getInstance()->openContext( this->selectDialog( dialogNum ) );
        }
        break;
    }
  }

}


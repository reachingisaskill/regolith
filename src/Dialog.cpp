
#include "Dialog.h"
#include "Manager.h"
#include "ObjectBuilder.h"
#include "Utilities.h"


namespace Regolith
{

  // Dialog Window

  DialogWindow::DialogWindow( Scene* scene, Camera* camera, Json::Value& data ) :
    _scene( scene ),
    _camera( camera ),
    _dialogTree( nullptr ),
    _currentDialog( nullptr )
  {
    this->loadFromJson( data );
  }


  void DialogWindow::loadFromJson( Json::Value& json_data )
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
        man->raiseEvent( REGOLITH_EVENT_QUIT );
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
    Utilities::validateJson( json_data, "background", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_ARRAY );

    try
    {
      // Load the scene background
      INFO_LOG( "Building the dialog background" );
      Json::Value& background_data = json_data["background"];
      Utilities::validateJson( background_data, "resource_name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( background_data, "position", Utilities::JSON_TYPE_ARRAY );

      _background = _owner->findResource( background_data["resource_name"].asString() )->clone();

      int x = background_data["position"][0].asInt();
      int y = background_data["position"][1].asInt();
      Vector pos( x, y );

      if ( _background->hasAnimation() )
      {
        _animated.push_back( _background );
      }


      // Load the children first
      INFO_LOG( "Building the child dialogs" );
      if ( json_data.isMember( "dialogs" ) && json_data["dialogs"].isArray() )
      {
        Json::Value& dialogs = json_data["dialogs"];
        Json::ArrayIndex dialogs_size = dialogs.size();
        for ( Json::ArrayIndex i = 0; i != dialogs_size; ++i )
        {
          Utilities::validateJson( dialogs[i], "name", Utilities::JSON_TYPE_STRING );
          std::string dialog_name = dialogs[i]["name"].asString();

          _subDialogs.addObject( new Dialog( _owner, _HUDCamera, dialogs[i] ), dialog_name );
        }
      }


      // Load the Scene Elements
      INFO_LOG( "Building the dialog scene" );
      Json::Value elements = json_data["elements"];
      Json::ArrayIndex elements_size = elements.size();
      for ( Json::ArrayIndex i = 0; i != elements_size; ++i )
      {
        Utilities::validateJson( elements[i], "resource_name", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( elements[i], "position", Utilities::JSON_TYPE_ARRAY );

        // Load the necessary info
        std::string resource_name = elements[i]["resource_name"].asString();
        int x = elements[i]["position"][0].asInt();
        int y = elements[i]["position"][1].asInt();
        Vector pos( x, y );

        // Determine the ID number and spawn the element
        Drawable* newElement = _owner->findResource( resource_name );
        _elements.push_back( newElement );
  
        // If animated, add it to the animated cache
        if ( newElement->hasAnimation() )
        {
          _animated.push_back( newElement );
        }

        // If it has interaction, try to cast it to a Button*
        if ( newElement->hasInteraction() )
        {
          Button* b_pointer = dynamic_cast< Button* >( newElement );
          if ( b_pointer != nullptr )
          {
            Utilities::validateJson( elements[i], "action", Utilities::JSON_TYPE_ARRAY );
            std::string action_name = elements[i]["action"].asString();

            // What is the base context id
            ContextEvent event_id = getContextEventID( action_name );
            if ( event_id == CONTEXT_EVENT_OPTIONS ) // Its not a standard context event => must be option specific
            {
              // Get the name of the corresponding sub dialog
              event_id = (ContextEvent)( (unsigned int)event_id + (unsigned int)_subDialogs.getID( action_name ) );
            }
            b_pointer->setOption( event_id );

            _buttons.addButton( b_pointer );
          }
        }
      }

    }
    catch ( Exception& ex )
    {
      if ( ex.isRecoverable() )
      {
        ERROR_STREAM << "An error occured building dialog: " << ex.what();
      }
      else
      {
        FAILURE_STREAM << "An error occured building dialog: " << ex.what();
        FAILURE_LOG( "Error is non-recoverable" );
        throw ex;
      }
    }
  }


  Dialog::~Dialog()
  {
    // Clear the caches
    _animated.clear();
    _buttons.clear();

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
  }


  void Dialog::render()
  {
    // First draw the background
    _background->render( _HUDCamera );

    // Render each element in the element list
    ElementList::iterator end = _elements.end();
    for ( ElementList::iterator it = _elements.begin(); it != end; ++it )
    {
      (*it)->render( _HUDCamera );
    }
  }


  Dialog* Dialog::selectDialog( unsigned int n )
  {
    return _subDialogs[ n ];
  }

}


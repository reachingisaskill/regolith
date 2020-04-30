
#include "Dialog.h"
#include "Manager.h"
#include "ObjectBuilder.h"
#include "Utilities.h"


namespace Regolith
{

  Dialog::Dialog( Scene* scene, Camera* camera, Json::Value& json_data ) :
    _owner( scene ),
    _HUDCamera( camera ),
    _name(),
    _subDialogs("sub_dialogs"),
    _elements(),
    _background(),
    _buttons()
  {
    this->loadFromJson( json_data );
  }


  void Dialog::loadFromJson( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );

    try
    {
      _name = json_data["name"].asString();


      INFO_LOG( "Checking for background sprite" );
      // Load the scene background
      if ( Utilities::validateJson( json_data, "background", Utilities::JSON_TYPE_STRING, false ) )
      {
        INFO_LOG( "Building the dialog background" );
        Json::Value& background_data = json_data["background"];
        Utilities::validateJson( background_data, "resource_name", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( background_data, "position", Utilities::JSON_TYPE_ARRAY );

        _background = _owner->findResource( background_data["resource_name"].asString() )->clone();

        int x = background_data["position"][0].asInt();
        int y = background_data["position"][1].asInt();
        Vector pos( x, y );

        _background->setPosition( pos );

        if ( _background->hasAnimation() )
        {
          INFO_LOG( "Dialog background is animated" );
          _animated.push_back( _background );
        }
      }
      else _background = nullptr;


      INFO_LOG( "Checking for child dialogs" );
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
        INFO_STREAM << "Loaded " << dialogs_size << " children";
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

        INFO_STREAM << "Loading resource : " << resource_name;

        // Determine the ID number and spawn the element
        Drawable* newElement = _owner->findResource( resource_name );
        newElement->setPosition( pos );
        _elements.push_back( newElement );
        INFO_LOG( "Resource loaded." );
  
        // If animated, add it to the animated cache
        if ( newElement->hasAnimation() )
        {
          INFO_STREAM << "Resource is animated";
          _animated.push_back( newElement );
        }

        // If it has interaction, try to cast it to a Button*
        if ( newElement->hasInteraction() )
        {
          INFO_LOG( "Resource is interactable." );

          Button* b_pointer = dynamic_cast< Button* >( newElement );
          if ( b_pointer != nullptr )
          {
            INFO_LOG( "Caching resource as a button" );

            Utilities::validateJson( elements[i], "action", Utilities::JSON_TYPE_ARRAY );
            std::string action_name = elements[i]["action"].asString();

            // What is the base context id
            InputAction action_id = getActionID( action_name );
            if ( action_id == INPUT_ACTION_OPTIONS ) // Its not a standard action => must be option specific
            {
              // Get the name of the corresponding sub dialog
              action_id = (InputAction)( (unsigned int)action_id + (unsigned int)_subDialogs.getID( action_name ) );
            }
            b_pointer->setOption( action_id );

            _buttons.addButton( b_pointer );
          }
        }
      }


      INFO_LOG( "Configuring Dialog Options" );
      // Check for the known options
      Json::Value options = json_data["options"];
      if ( Utilities::validateJson( options, "can_cancel", Utilities::JSON_TYPE_BOOLEAN, false ) )
      {
        _canCancel = options["can_cancel"].asBool();
      }

      INFO_LOG( "Configuring action handler for dialog" );
      this->registerActions( inputHandler() );
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
    DEBUG_LOG( "Rendering Dialog" );
    // First draw the background
    if ( _background != nullptr )
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


  void Dialog::raiseContextEvent( ContextEvent )
  {
  }


  void Dialog::registerActions( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_QUIT );
    handler->registerInputRequest( this, INPUT_ACTION_NEXT );
    handler->registerInputRequest( this, INPUT_ACTION_PREV );
    handler->registerInputRequest( this, INPUT_ACTION_SELECT );
    handler->registerInputRequest( this, INPUT_ACTION_PAUSE );
    handler->registerInputRequest( this, INPUT_ACTION_BACK );
    handler->registerInputRequest( this, INPUT_ACTION_CANCEL );
    handler->registerInputRequest( this, INPUT_ACTION_JUMP ); // Bind the jump key just in case!
    handler->registerInputRequest( this, INPUT_ACTION_CLICK ); // Bind the mouse click
  }


//  void Dialog::eventAction( const RegolithEvent& event )
//  {
//  }


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
        if ( _canCancel && value ) this->takeFocus();
        break;

      default :
        break;
    }
  }


//  void Dialog::floatAction( const InputAction& action, float )
//  {
//  }


  void Dialog::vectorAction( const InputAction& action, const Vector& vector )
  {
  }


}


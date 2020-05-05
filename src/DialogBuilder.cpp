
#include "DialogBuilder.h"
#include "Dialog.h"
#include "Utilities.h"
#include "Manager.h"
#include "Signal.h"

#include "logtastic.h"


namespace Regolith
{



////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Builder

  DialogBuilder::~DialogBuilder()
  {
    for ( FactoryMap::iterator it = _factories.begin(); it != _factories.end(); ++it )
    {
      delete it->second;
    }
    _factories.clear();
  }


  void DialogBuilder::addFactory( DialogFactory* newFactory )
  {
    std::string name( newFactory->getDialogName() );

    if ( _factories.find( name ) != _factories.end() )
    {
      ERROR_LOG( "Attempting to add dialog factory with a name that is already loaded" );
      return;
    }

    newFactory->_parent = this;

    _factories[name] = newFactory;
  }


  Dialog* DialogBuilder::build( Json::Value& json_data, Camera* theCamera ) const
  {
    std::string dialog_type = json_data["dialog_type"].asString();
    INFO_STREAM << "Building dialog of type : " << dialog_type;

    DialogBuilder::FactoryMap::const_iterator found = _factories.find( dialog_type );

    if ( found == _factories.end() )
    {
      ERROR_STREAM << "Dialog type not found in factory list: " << dialog_type;
      Exception ex( "DialogBuilder::build()", "Non-implemented build type", true );
      ex.addDetail( "TypeID", dialog_type );
      throw ex;
    }

    return found->second->build( json_data, theCamera );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Factory Base


  void DialogFactory::buildBackground( Dialog* dialog, Camera* camera, Json::Value& json_data ) const
  {
    // Load the scene background
    if ( Utilities::validateJson( json_data, "background", Utilities::JSON_TYPE_STRING, false ) )
    {
      INFO_LOG( "Building the dialog background" );
      Json::Value& background_data = json_data["background"];
      Utilities::validateJson( background_data, "resource_name", Utilities::JSON_TYPE_STRING );
      dialog->_background = Manager::getInstance()->spawn( background_data["resource_name"].asString(), Vector( 0, 0 ) );

      Utilities::jsonProcessPosition( background_data, dialog->_background, camera );

      if ( dialog->_background->hasAnimation() )
      {
        INFO_LOG( "Dialog background is animated" );
        dialog->_animated.push_back( dialog->_background );
      }
    }
    else 
    {
      dialog->_background = nullptr;
    }
  }



  void DialogFactory::buildElements( Dialog* dialog, Camera* camera, Json::Value& elements ) const
  {
    INFO_LOG( "Building the dialog scene" );
    Json::ArrayIndex elements_size = elements.size();
    for ( Json::ArrayIndex i = 0; i != elements_size; ++i )
    {
      Utilities::validateJson( elements[i], "resource_name", Utilities::JSON_TYPE_STRING );

      // Load the necessary info
      std::string resource_name = elements[i]["resource_name"].asString();
      INFO_STREAM << "Loading resource : " << resource_name;

      // Determine the ID number and spawn the element
      Drawable* newElement = Manager::getInstance()->spawn( resource_name );
      DEBUG_STREAM << "ELEMENT = " << newElement;

      Utilities::jsonProcessPosition( elements[i], newElement, camera );
      dialog->_elements.push_back( newElement );
      INFO_LOG( "Resource loaded." );

      // If animated, add it to the animated cache
      if ( newElement->hasAnimation() )
      {
        INFO_STREAM << "Resource is animated";
        dialog->_animated.push_back( newElement );
      }

      // If it has interaction, try to cast it to a Button*
      if ( newElement->hasInteraction() )
      {
        INFO_LOG( "Resource is interactable." );

        Button* b_pointer = dynamic_cast< Button* >( newElement );
        if ( b_pointer != nullptr )
        {
          INFO_LOG( "Caching resource as a button" );

          if ( Utilities::validateJson( elements[i], "actions", Utilities::JSON_TYPE_ARRAY, false ) )
          {
            Json::Value actions = elements[i]["actions"];
            Json::ArrayIndex actions_size = actions.size();
            for ( Json::ArrayIndex j = 0; j < actions_size; ++j )
            {
              b_pointer->addAction( makeSignal( actions[j], dialog ) );
            }
          }
          else
          {
            WARN_LOG( "No action assigned to this button" );
          }

          dialog->_buttons.addButton( b_pointer );
        }
      }
    }
  }


  void DialogFactory::buildOptions( Dialog* dialog, Camera*, Json::Value& options ) const
  {
    INFO_LOG( "Configuring Dialog Options" );
    if ( Utilities::validateJson( options, "can_cancel", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      dialog->_canCancel = options["can_cancel"].asBool();
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Menu Factory

  Dialog* MenuDialogFactory::build( Json::Value& json_data, Camera* theCamera ) const
  {
    INFO_LOG( "Building Menu Dialog" );
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "input_mapping", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "elements", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "options", Utilities::JSON_TYPE_OBJECT );

    MenuDialog* theDialog = nullptr;

    try
    {
      std::string name = json_data["name"].asString();
      std::string mapping = json_data["input_mapping"].asString();

      theDialog = new MenuDialog( theCamera, mapping );


      INFO_LOG( "Checking for background sprite" );
      this->buildBackground( theDialog, theCamera, json_data );


      // Load the children first
      INFO_LOG( "Building the child dialogs" );
      if ( Utilities::validateJson( json_data, "dialogs", Utilities::JSON_TYPE_ARRAY, false ) )
      {
        // Get a pointer to the builder
        DialogBuilder* builder = this->getParent();

        Json::Value& dialogs = json_data["dialogs"];
        Json::ArrayIndex dialogs_size = dialogs.size();
        for ( Json::ArrayIndex i = 0; i != dialogs_size; ++i )
        {
          Utilities::validateJson( dialogs[i], "name", Utilities::JSON_TYPE_STRING );
          std::string name = dialogs[i]["name"].asString();

          Dialog* childDialog = builder->build( dialogs[i], theCamera );

          theDialog->_subDialogs.addObject( childDialog, name );
          theDialog->registerContextEvent( name );
        }
        INFO_STREAM << "Loaded " << dialogs_size << " children";
      }


      // Load the Scene Elements
      this->buildElements( theDialog, theCamera, json_data["elements"] );

      this->buildOptions( theDialog, theCamera, json_data["options"] );

      INFO_LOG( "Configuring action handler for dialog" );
      theDialog->registerActions( theDialog->inputHandler() );

      DEBUG_STREAM << "Build Dialog: " << name << " @ " << theDialog;
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
    DEBUG_STREAM << "Build Dialog @ " << theDialog;

    return theDialog;
  }

}


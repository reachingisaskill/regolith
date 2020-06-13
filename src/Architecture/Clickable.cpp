
#include "Regolith/Architecture/Clickable.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  void Clickable::giveFocus()
  {
    switch ( _state )
    {
      case NORMAL :
        this->onStateChange( FOCUSSED );
        _state = FOCUSSED;
        break;

      case FOCUSSED :
      case DOWN :
      case INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON FOCUSSED" );
  }


  void Clickable::takeFocus()
  {
    switch ( _state )
    {
      case FOCUSSED :
      case DOWN :
        this->onStateChange( NORMAL );
        _state = NORMAL;
        break;

      case NORMAL :
      case INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON DEFOCUSSED" );
  }


  void Clickable::down()
  {
    switch ( _state )
    {
      case FOCUSSED :
      case NORMAL :
        this->onStateChange( DOWN );
        _state = DOWN;
        break;

      case DOWN :
      case INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON DOWN" );
  }


  void Clickable::up()
  {
    switch ( _state )
    {
      case DOWN : // Only trigger when moving from down->up
        this->click();
        this->onStateChange( FOCUSSED );
        _state = FOCUSSED;
        break;

      case FOCUSSED :
        this->onStateChange( NORMAL );
        _state = NORMAL;
        break;

      case NORMAL :
      case INACTIVE :
      default :
        break;
    }
    DEBUG_LOG ( "BUTTON UP" );
  }


  void Clickable::activate()
  {
    switch ( _state )
    {
      case NORMAL :
      case FOCUSSED :
      case DOWN :
        break;

      case INACTIVE :
        this->onStateChange( NORMAL );
        _state = NORMAL;
        break;

      default :
        break;
    }
  }


  void Clickable::deactivate()
  {
    switch ( _state )
    {
      case NORMAL :
      case FOCUSSED :
      case DOWN :
        this->onStateChange( INACTIVE );
        _state = INACTIVE;
        break;

      case INACTIVE :
      default :
        break;
    }
  }


  void Clickable::configure( Json::Value& json_data, ContextGroup&, DataHandler& )
  {
    if ( Utilities::validateJson( json_data, "clickable_state", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string state_string = json_data["clickable_state"].asString();

      if ( state_string == "inactive" )
      {
        _state = INACTIVE;
      }
      else
      {
        _state = NORMAL;
      }
    }
  }

}



#include "Regolith/GamePlay/Signal.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Event Signal

  InputActionSignal::InputActionSignal( InputAction action ) :
    _theAction( action )
  {
  }


  void InputActionSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateInputAction( man->getCurrentContext()->inputHandler(), _theAction );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Boolean Action Signal

  InputBooleanSignal::InputBooleanSignal( InputAction action, bool val ) :
    _theAction( action ),
    _theValue( val )
  {
  }


  void InputBooleanSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateBooleanAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Float Action Signal

  InputFloatSignal::InputFloatSignal( InputAction action, float val ) :
    _theAction( action ),
    _theValue( val )
  {
  }


  void InputFloatSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateFloatAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vector Action Signal

  InputVectorSignal::InputVectorSignal( InputAction action, Vector val ) :
    _theAction( action ),
    _theValue( val )
  {
  }


  void InputVectorSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateVectorAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Mouse Action Signal

  InputMouseSignal::InputMouseSignal( InputAction action, Vector val, bool click ) :
    _theAction( action ),
    _position( val ),
    _click( click )
  {
  }


  void InputMouseSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateMouseAction( man->getCurrentContext()->inputHandler(), _theAction, _click, _position );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Event Signal

  GameEventSignal::GameEventSignal( RegolithEvent event ) :
    _theEvent( event )
  {
  }


  void GameEventSignal::trigger() const
  {
    Manager::getInstance()->raiseEvent( _theEvent );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
//  // Context Event Signal
//
//  ContextEventSignal::ContextEventSignal( ContextEvent event ) :
//    _theEvent( event )
//  {
//  }
//
//
//  void ContextEventSignal::trigger() const
//  {
//    Manager::getInstance()->getCurrentContext()->raiseContextEvent( _theEvent );
//  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Change Signal

  ChangeContextSignal::ChangeContextSignal( Engine::StackOperation::Operation op, Context* cont ) :
    _theContext( cont ),
    _operation( op )
  {
  }


  void ChangeContextSignal::trigger() const
  {
    switch ( _operation )
    {
      case Engine::StackOperation::POP :
        Manager::getInstance()->closeContext();
        break;

      case Engine::StackOperation::PUSH :
        Manager::getInstance()->openContext( _theContext );
        break;

      case Engine::StackOperation::RESET :
        Manager::getInstance()->setContextStack( _theContext );
        break;

      case Engine::StackOperation::TRANSFER :
        Manager::getInstance()->transferContext( _theContext );
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Building functions

  Signal* makeSignal( Json::Value& json_data, Context* /*context*/ )
  {
    Signal* newSignal = nullptr;

    Utilities::validateJson( json_data, "type", Utilities::JSON_TYPE_STRING );

    std::string type = json_data["type"].asString();
    INFO_LOG( "Building Signal" );

    if ( type == "input_action" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
      newSignal = new InputActionSignal( getActionID( json_data["action"].asString() ) );
    }
    else if ( type == "input_boolean" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "value", Utilities::JSON_TYPE_BOOLEAN );
      InputAction action = getActionID( json_data["action"].asString() );
      bool value = json_data["value"].asBool();

      newSignal = new InputBooleanSignal( action, value );
    }
    else if ( type == "input_float" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "value", Utilities::JSON_TYPE_FLOAT );
      InputAction action = getActionID( json_data["action"].asString() );
      bool value = json_data["value"].asFloat();

      newSignal = new InputFloatSignal( action, value );
    }
    else if ( type == "input_vector" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "vector", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJsonArray( json_data, 2, Utilities::JSON_TYPE_FLOAT );

      InputAction action = getActionID( json_data["action"].asString() );
      Vector value( json_data["vector"][0].asFloat(), json_data["vector"][1].asFloat() );

      newSignal = new InputVectorSignal( action, value );
    }
    else if ( type == "input_mouse" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY );
      Utilities::validateJson( json_data, "click", Utilities::JSON_TYPE_BOOLEAN );
      Utilities::validateJsonArray( json_data, 2, Utilities::JSON_TYPE_FLOAT );

      InputAction action = getActionID( json_data["action"].asString() );
      Vector position( json_data["vector"][0].asFloat(), json_data["vector"][1].asFloat() );
      bool click = json_data["click"].asBool();

      newSignal = new InputMouseSignal( action, position, click );
    }
    else if ( type == "game_event" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "event", Utilities::JSON_TYPE_STRING );
      newSignal = new GameEventSignal( getRegolithEventID( json_data["event"].asString() ) );
    }
//    else if ( type == "context_event" )
//    {
//      INFO_STREAM << " Signal type: " << type;
//      Utilities::validateJson( json_data, "event", Utilities::JSON_TYPE_STRING );
//      newSignal = new ContextEventSignal( context->getContextEventID( json_data["event"].asString() ) );
//    }
    else if ( type == "change_context" )
    {
      INFO_STREAM << " Signal type: " << type;
      Utilities::validateJson( json_data, "context_name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( json_data, "operation", Utilities::JSON_TYPE_STRING );

      Engine::StackOperation::Operation op;
      Context* context = nullptr;

      if ( json_data["operation"].asString() == "open" )
      {
        op = Engine::StackOperation::PUSH;
        context = Manager::getInstance()->getContext( json_data["context_name"].asString() );
      }
      if ( json_data["operation"].asString() == "close" )
      {
        op = Engine::StackOperation::POP;
        // No context name needed.
      }
      if ( json_data["operation"].asString() == "transfer" )
      {
        op = Engine::StackOperation::TRANSFER;
        context = Manager::getInstance()->getContext( json_data["context_name"].asString() );
      }
      if ( json_data["operation"].asString() == "reset" )
      {
        op = Engine::StackOperation::RESET;
        context = Manager::getInstance()->getContext( json_data["context_name"].asString() );
      }

      newSignal = new ChangeContextSignal( op, context );
    }
    else
    {
      ERROR_STREAM << "Unknown type of signal requested: " << type;
    }

    return newSignal;
  }

}


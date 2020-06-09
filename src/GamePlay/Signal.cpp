
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

  InputActionSignal::InputActionSignal() :
    _theAction()
  {
  }


  void InputActionSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateInputAction( man->getCurrentContext()->inputHandler(), _theAction );
  }


  void InputActionSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Input Action Signal" );
    Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
    _theAction = getActionID( json_data["action"].asString() );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Boolean Action Signal

  InputBooleanSignal::InputBooleanSignal() :
    _theAction(),
    _theValue()
  {
  }


  void InputBooleanSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateBooleanAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }

  void InputBooleanSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Input Boolean Signal" );
    Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "value", Utilities::JSON_TYPE_BOOLEAN );

    InputAction action = getActionID( json_data["action"].asString() );
    bool value = json_data["value"].asBool();

    _theAction = action;
    _theValue = value;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Float Action Signal

  InputFloatSignal::InputFloatSignal() :
    _theAction(),
    _theValue()
  {
  }


  void InputFloatSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateFloatAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }


  void InputFloatSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Input Float Signal" );
    Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "value", Utilities::JSON_TYPE_FLOAT );

    InputAction action = getActionID( json_data["action"].asString() );
    bool value = json_data["value"].asFloat();

    _theAction = action;
    _theValue = value;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vector Action Signal

  InputVectorSignal::InputVectorSignal() :
    _theAction(),
    _theValue()
  {
  }


  void InputVectorSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateVectorAction( man->getCurrentContext()->inputHandler(), _theAction, _theValue );
  }


  void InputVectorSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Input Vector Signal" );
    Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "vector", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data, 2, Utilities::JSON_TYPE_FLOAT );

    InputAction action = getActionID( json_data["action"].asString() );
    Vector value( json_data["vector"][0].asFloat(), json_data["vector"][1].asFloat() );

    _theAction = action;
    _theValue = value;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Mouse Action Signal

  InputMouseSignal::InputMouseSignal() :
    _theAction(),
    _position(),
    _click()
  {
  }


  void InputMouseSignal::trigger() const
  {
    Manager* man = Manager::getInstance();
    man->getInputManager().simulateMouseAction( man->getCurrentContext()->inputHandler(), _theAction, _click, _position );
  }


  void InputMouseSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Input Mouse Signal" );
    Utilities::validateJson( json_data, "action", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "click", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJsonArray( json_data, 2, Utilities::JSON_TYPE_FLOAT );

    InputAction action = getActionID( json_data["action"].asString() );
    Vector position( json_data["vector"][0].asFloat(), json_data["vector"][1].asFloat() );
    bool click = json_data["click"].asBool();

    _theAction = action;
    _position = position;
    _click = click;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Event Signal

  GameEventSignal::GameEventSignal() :
    _theEvent()
  {
  }


  void GameEventSignal::trigger() const
  {
    Manager::getInstance()->raiseEvent( _theEvent );
  }


  void GameEventSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    INFO_LOG( " Game Event Signal" );
    Utilities::validateJson( json_data, "event", Utilities::JSON_TYPE_STRING );

    _theEvent = getRegolithEventID( json_data["event"].asString() );
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

  ChangeContextSignal::ChangeContextSignal() :
    _theContext(),
    _operation()
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
        Manager::getInstance()->openContext( *_theContext );
        break;

      case Engine::StackOperation::RESET :
        Manager::getInstance()->setContextStack( *_theContext );
        break;

      case Engine::StackOperation::TRANSFER :
        Manager::getInstance()->transferContext( *_theContext );
        break;
    }
  }


  void ChangeContextSignal::configure( Json::Value& json_data, ContextGroup& cg )
  {
    INFO_LOG( " Change Context Signal" );
    Utilities::validateJson( json_data, "context_name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "operation", Utilities::JSON_TYPE_STRING );

    Engine::StackOperation::Operation op;
    Proxy<Context*> context;

    if ( json_data["operation"].asString() == "open" )
    {
      op = Engine::StackOperation::PUSH;
      context = cg.requestContext( json_data["context_name"].asString() );
    }
    if ( json_data["operation"].asString() == "close" )
    {
      op = Engine::StackOperation::POP;
      // No context name needed.
    }
    if ( json_data["operation"].asString() == "transfer" )
    {
      op = Engine::StackOperation::TRANSFER;
      context = cg.requestContext( json_data["context_name"].asString() );
    }
    if ( json_data["operation"].asString() == "reset" )
    {
      op = Engine::StackOperation::RESET;
      context = cg.requestContext( json_data["context_name"].asString() );
    }

    _operation = op;
    _theContext = context;
  }


  void ChangeContextSignal::validate() const
  {
    if ( ! _theContext || ( (*_theContext) == nullptr ) )
    {
      Exception ex( "ChangeContextSignal::validate() const", "Specified context was not configured." );
      ex.addDetail( "Context Name", _theContext.getName() );
      throw ex;
    }
  }

}


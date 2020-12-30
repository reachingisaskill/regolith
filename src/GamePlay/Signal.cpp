
#include "Regolith/GamePlay/Signal.h"

#include "Regolith/Contexts/Context.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Utilities/JsonValidation.h"


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
    validateJson( json_data, "action", JsonType::STRING );
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
    validateJson( json_data, "action", JsonType::STRING );
    validateJson( json_data, "value", JsonType::BOOLEAN );

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
    validateJson( json_data, "action", JsonType::STRING );
    validateJson( json_data, "value", JsonType::FLOAT );

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
    validateJson( json_data, "action", JsonType::STRING );
    validateJson( json_data, "vector", JsonType::ARRAY );
    validateJsonArray( json_data, 2, JsonType::FLOAT );

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
    validateJson( json_data, "action", JsonType::STRING );
    validateJson( json_data, "position", JsonType::ARRAY );
    validateJson( json_data, "click", JsonType::BOOLEAN );
    validateJsonArray( json_data, 2, JsonType::FLOAT );

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
    validateJson( json_data, "event", JsonType::STRING );

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

  OpenContextSignal::OpenContextSignal() :
    _theContext()
  {
  }


  void OpenContextSignal::trigger() const
  {
    DEBUG_LOG( "OpenContextSignal::trigger : Opening context" );
    Manager::getInstance()->openContext( *_theContext );
  }


  void OpenContextSignal::configure( Json::Value& json_data, ContextGroup& cg )
  {
    validateJson( json_data, "context_name", JsonType::STRING );
    _theContext = cg.getContextPointer( json_data["context_name"].asString() );

    INFO_STREAM << "OpenContextSignal::configure : Registered context with name: " << json_data["context_name"].asString();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Stack Change Signal

  OpenContextStackSignal::OpenContextStackSignal() :
    _theContext()
  {
  }


  void OpenContextStackSignal::trigger() const
  {
    DEBUG_LOG( "OpenContextStackSignal::trigger : Opening context" );
    Manager::getInstance()->openContextStack( *_theContext );
  }


  void OpenContextStackSignal::configure( Json::Value& json_data, ContextGroup& cg )
  {
    validateJson( json_data, "context_name", JsonType::STRING );
    _theContext = cg.getContextPointer( json_data["context_name"].asString() );

    INFO_STREAM << "OpenContextStackSignal::configure : Registered context with name: " << json_data["context_name"].asString();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Group Change Signal

  OpenContextGroupSignal::OpenContextGroupSignal() :
    _theContextGroup( nullptr )
  {
  }


  void OpenContextGroupSignal::trigger() const
  {
    DEBUG_LOG( "OpenContextGroupSignal::trigger : Opening context group" );
    Manager::getInstance()->openContextGroup( _theContextGroup );
  }


  void OpenContextGroupSignal::configure( Json::Value& json_data, ContextGroup& )
  {
    validateJson( json_data, "context_group", JsonType::STRING );
    _theContextGroup = Manager::getInstance()->getContextManager().getContextGroup( json_data["context_group"].asString() );

    INFO_STREAM << "OpenContextGroupSignal::configure : Registered context group with name : " << json_data["context_group"].asString();
  }

}


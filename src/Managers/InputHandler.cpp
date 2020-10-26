#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/InputHandler.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  InputHandler::InputHandler() :
    _inputMaps( nullptr ),
    _actionMaps()
  {
  }


  InputHandler::~InputHandler()
  {
  }


  void InputHandler::configure( std::string mappingName )
  {
    _inputMaps = Manager::getInstance()->getInputManager().requestMapping( mappingName );
    DEBUG_STREAM << "InputHandler::configure : Requested Input Map Pointer: " << _inputMaps;
  }


  void InputHandler::registerInputRequest( ControllableInterface* object, InputAction action )
  {
    DEBUG_STREAM << "InputHandler::registerInputRequest : Registered input request for action: " << action << " " << object;
    _actionMaps[action].insert( object );
  }

}


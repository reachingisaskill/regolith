
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
  }


  void InputHandler::registerInputRequest( ControllableInterface* object, InputAction action )
  {
    INFO_STREAM << "Registered input request for action: " << action << " " << object;
    _actionMaps[action].insert( object );
  }

}


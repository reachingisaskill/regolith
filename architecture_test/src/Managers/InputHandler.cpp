
#include "Regolith/Managers/InputHandler.h"

namespace Regolith
{

  InputHandler::InputHandler( std::string mappingName ) :
    _inputMaps( Manager::getInstance()->getInputManager().requestMapping( mappingName ) ),
    _actionMaps()
  {
  }


  InputHandler::~InputHandler()
  {
  }


  void InputHandler::registerInputRequest( ControllableInterface* object, InputAction action )
  {
    INFO_STREAM << "Registered input request for action: " << action << " " << object;
    _actionMaps[action].insert( object );
  }

}


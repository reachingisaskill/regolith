
#include "Regolith/GameObjects/ControllableCharacter.h"


namespace Regolith
{

  void ControllableCharacter::configure( Json::Value& json_data, DataHandler& handler )
  {
    Controllable::configure( json_data, handler );
    CharacterBase::configure( json_data, handler );
  }

}



#include "Regolith/GameObjects/ControllableCharacter.h"


namespace Regolith
{

  void ControllableCharacter::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Controllable::configure( json_data, cg, handler );
    CharacterBase::configure( json_data, cg, handler );
  }

}


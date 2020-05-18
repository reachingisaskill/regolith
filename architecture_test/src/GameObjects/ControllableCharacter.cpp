
#include "Regolith/GameObjects/ControllableCharacter.h"


namespace Regolith
{

  void ControllableCharacter::configure( Json::Value& json_data )
  {
    Controllable::configure( json_data );
    CharacterBase::configure( json_data );
  }

}


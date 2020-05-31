
#include "Regolith/GameObjects/CharacterBase.h"
#include "Regolith/Managers/DataHandler.h"

namespace Regolith
{

  CharacterBase::CharacterBase() :
    Drawable(),
    Animated(),
    Moveable(),
    Noisy(),
    Collidable()
  {
  }


  CharacterBase::~CharacterBase()
  {
  }


  void CharacterBase::configure( Json::Value& json_data, DataHandler& handler )
  {
    Drawable::configure( json_data, handler );
    Animated::configure( json_data, handler );
    Moveable::configure( json_data, handler );
    Noisy::configure( json_data, handler );
    Collidable::configure( json_data, handler );
  }

}


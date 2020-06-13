
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


  void CharacterBase::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Drawable::configure( json_data, cg, handler );
    Animated::configure( json_data, cg, handler );
    Moveable::configure( json_data, cg, handler );
    Noisy::configure( json_data, cg, handler );
    Collidable::configure( json_data, cg, handler );
  }

}


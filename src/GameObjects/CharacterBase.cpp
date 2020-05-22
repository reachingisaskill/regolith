
#include "Regolith/GameObjects/CharacterBase.h"

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


  void CharacterBase::configure( Json::Value& json_data )
  {
    Drawable::configure( json_data );
    Animated::configure( json_data );
    Moveable::configure( json_data );
    Noisy::configure( json_data );
    Collidable::configure( json_data );
  }

}


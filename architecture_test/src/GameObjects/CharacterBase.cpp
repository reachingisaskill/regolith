
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
  }

}


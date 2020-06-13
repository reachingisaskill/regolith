
#ifndef REGOLITH_GAMEOBJECTS_CHARACTERBASE_H_
#define REGOLITH_GAMEOBJECTS_CHARACTERBASE_H_ 

#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Animated.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Noisy.h"
#include "Regolith/Architecture/Collidable.h"

namespace Regolith
{
  class DataHandler;

  class CharacterBase : public Drawable, public Animated, public Moveable, public Noisy, public Collidable
  {
    private:

    public:
      // Con/De-struction
      CharacterBase();
      virtual ~CharacterBase();

      // Configuration
      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override;

      // Replace the character at the specified position
      virtual void respawn( const Vector& ) = 0;

  };

}

#endif // REGOLITH_GAMEOBJECTS_CHARACTERBASE_H_


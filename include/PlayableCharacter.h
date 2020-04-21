
#ifndef __REGOLITH__PLAYABLE_CHARACTER_H__
#define __REGOLITH__PLAYABLE_CHARACTER_H__

#include "Definitions.h"

#include "Drawable.h"
#include "Camera.h"
#include "Collision.h"


namespace Regolith
{


  class PlayableCharacter : public Drawable
  {
    private:

    public:
      PlayableCharacter();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual int getProperties() const { return OBJECT_ANIMATED & OBJECT_HAS_COLLISION & OBJECT_HAS_INPUT; }

      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* );

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 );

      // Update the objects behaviour based on the provided timestep
      virtual void handleEvent( SDL_Event& );

      // Returns the collision object for the class;
      virtual Collision* getCollision();

  };

}

#endif // __REGOLITH__PLAYABLE_CHARACTER_H__



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
      virtual bool hasCollision() const { return true; }
      virtual bool hasInput() const { return true; }
      virtual bool hasAnimation() const { return true; }

      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* );

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 );

      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputHandler* );

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& );

      // Return a new clone of the current object. Transfers ownership of the memory
      Drawable* clone() const { return (Drawable*) new PlayableCharacter( *this ); }

  };

}

#endif // __REGOLITH__PLAYABLE_CHARACTER_H__


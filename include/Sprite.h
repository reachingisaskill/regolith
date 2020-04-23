
#ifndef __REGOLITH__SPRITE_H__
#define __REGOLITH__SPRITE_H__

#include "Definitions.h"

#include "SpriteSheet.h"


namespace Regolith
{
  class Camera;

  class Sprite : public Drawable
  {
    private:
      SpriteSheet _spriteSheet;
      SDL_Rect _destination;

    protected:

    public:
      Sprite();

      explicit Sprite( SpriteSheet );

      virtual ~Sprite();


      // Return the object properties
      virtual int getProperties() const { return OBJECT_SIMPLE; }

      // For derived classes to update every frame
      virtual void update( Uint32 ) { }

      // Render with the current renderer object
      virtual void render( Camera* );

      // Handle events
      virtual void handleEvent( SDL_Event& ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& ) { return 0; }

      // Return a new clone of the current object. Transfers ownership of memory
      virtual Drawable* clone() const;


      // Sprite details
      SpriteSheet& spriteSheet() { return _spriteSheet; }
  };

}

#endif // __REGOLITH__SPRITE_H__


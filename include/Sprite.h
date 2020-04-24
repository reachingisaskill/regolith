
#ifndef __REGOLITH__SPRITE_H__
#define __REGOLITH__SPRITE_H__

#include "Definitions.h"

#include "Drawable.h"
#include "SpriteSheet.h"
#include "Collision.h"


namespace Regolith
{
  class Camera;

  class Sprite : public Drawable
  {
    private:
      SpriteSheet _spriteSheet;
      SDL_Rect _destination;
      Collision* _collision;

    protected:

    public:
      Sprite();

      explicit Sprite( SpriteSheet );

      Sprite( const Sprite& );

      virtual ~Sprite();


      // Return the object properties
      virtual bool hasCollision() const { return _collision != nullptr; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return this->isMovable(); }

      // For derived classes to update every frame
//      virtual void update( Uint32 );

      // Render with the current renderer object
      virtual void render( Camera* );

      // Handle events
      virtual void handleEvent( SDL_Event& ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& );

      // Return a new clone of the current object. Transfers ownership of memory
      virtual Drawable* clone() const;


      void setCollision( Collision* col ) { _collision = col; }

      // Sprite details
      SpriteSheet& spriteSheet() { return _spriteSheet; }
  };

}

#endif // __REGOLITH__SPRITE_H__


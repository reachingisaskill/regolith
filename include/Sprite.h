
#ifndef __REGOLITH__SPRITE_H__
#define __REGOLITH__SPRITE_H__

#include "Definitions.h"

#include "Texture.h"
#include "Drawable.h"
#include "Collision.h"


namespace Regolith
{
  class Camera;

  class Sprite : public Drawable
  {
    private:
      Texture _texture;
      SDL_Rect _destination;
      Collision* _collision;

    protected:

    public:
      Sprite();

      explicit Sprite( Texture );

      Sprite( const Sprite& );

      virtual ~Sprite();


      // Return the object properties
      virtual bool hasCollision() const { return _collision != nullptr; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return this->isMovable() || _texture.isAnimated(); }

      // For derived classes to update every frame
      virtual void update( Uint32 );

      // Render with the current renderer object
      virtual void render( Camera* );

      // Handle events
      virtual void registerEvents( InputHandler* ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& );

      // Return a new clone of the current object. Transfers ownership of memory
      virtual Drawable* clone() const;


      void addCollision( Collision* col ) { _collision = col; }

      // Sprite details
      Texture& texture() { return _texture; }
  };

}

#endif // __REGOLITH__SPRITE_H__


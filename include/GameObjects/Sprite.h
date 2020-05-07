
#ifndef REGOLITH_SPRITE_H_
#define REGOLITH_SPRITE_H_

#include "Managers/Definitions.h"
#include "Components/Texture.h"
#include "Components/Collision.h"
#include "Architecture/Drawable.h"


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
      virtual void registerActions( InputHandler* ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& );

      // Return a new clone of the current object. Transfers ownership of memory
      virtual Drawable* clone() const;


      // Set the collision object
      void addCollision( Collision* col );

      // Sprite details
      Texture& texture() { return _texture; }
  };

}

#endif // REGOLITH_SPRITE_H_


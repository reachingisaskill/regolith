
#ifndef __REGOLITH__DRAWABLE_H__
#define __REGOLITH__DRAWABLE_H__

#include "Definitions.h"

#include <SDL2/SDL.h>

namespace Regolith
{

  class Camera;
  class Collision;

  /*
   * Defines the interface for all objects that can be drawn by the rendering functions.
   */
  class Drawable
  {
    Drawable( const Drawable& ) = delete; // Non-copy-constructable
    Drawable& operator=( const Drawable& ) = delete; // Non-copy-assignable

    private:
      SDL_Renderer* _theRenderer;

    public:
      Drawable();

      Drawable( SDL_Renderer* );

      Drawable( const Drawable&& );

      Drawable& operator=( const Drawable&& );

      virtual ~Drawable();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual ObjectProperty getProperties() const = 0;

      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* ) = 0;

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 ) = 0;

      // Returns the collision object for the class;
      virtual Collision* getCollision() = 0;


      // Get and Set the Renderer object
      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }
      SDL_Renderer* getRenderer() const { return _theRenderer; }


      // Have to define some width and height values
      virtual int getWidth() const { return 0; }
      virtual int getHeight() const { return 0; }
  };

}

#endif // __REGOLITH__DRAWABLE_H__


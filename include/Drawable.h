
#ifndef __REGOLITH__DRAWABLE_H__
#define __REGOLITH__DRAWABLE_H__

#include "Definitions.h"


namespace Regolith
{

  class Camera;
  class Collision;

  /*
   * Defines the interface for all objects that can be drawn by the rendering functions.
   */
  class Drawable
  {

    private:
      SDL_Renderer* _theRenderer;

    public:
      Drawable();

      Drawable( SDL_Renderer* );

      Drawable( const Drawable& ) = default;
      Drawable( Drawable&& ) = default;
      Drawable& operator=( const Drawable& ) = default;
      Drawable& operator=( Drawable&& ) = default;

      virtual ~Drawable();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual int getProperties() const = 0;

      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* ) = 0;

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 ) = 0;

      // Update the objects behaviour based on the provided timestep
      virtual void handleEvent( SDL_Event& ) = 0;

      // Returns the number of collision objects for the class.
      // The argument is a pointer which will be updated to point at the first object
      virtual unsigned int getCollision( Collision*& ) = 0;


      // Get and Set the Renderer object
      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }
      SDL_Renderer* getRenderer() const { return _theRenderer; }


      // Have to define some width and height values
      virtual int getWidth() const { return 0; }
      virtual int getHeight() const { return 0; }
  };

}

#endif // __REGOLITH__DRAWABLE_H__


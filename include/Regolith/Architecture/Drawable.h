
#ifndef REGOLITH_ARCHITECTURE_DRAWABLE_H_
#define REGOLITH_ARCHITECTURE_DRAWABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  class Camera;

  /*
   * Defines the interface for all objects that can be drawn by the rendering functions.
   */
  class Drawable : virtual public PhysicalObject
  {
    public:
      Drawable();

      virtual ~Drawable() {}


      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override {}



      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual bool hasTexture() const override { return true; }



      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( const Camera&, SDL_Renderer* ) const = 0;

  };

}

#endif // REGOLITH_ARCHITECTURE_DRAWABLE_H_


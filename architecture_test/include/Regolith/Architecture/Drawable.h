
#ifndef REGOLITH_ARCHITECTURE_DRAWABLE_H_
#define REGOLITH_ARCHITECTURE_DRAWABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  class Camera;

  namespace Architecture
  {

    /*
     * Defines the interface for all objects that can be drawn by the rendering functions.
     */
    class Drawable : virtual public PhysicalObject
    {

      private:
        SDL_Renderer* _theRenderer;

      protected:
        void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }

      public:
        Drawable();

        Drawable( SDL_Renderer* );

        virtual ~Drawable() {}


        // Get and Set the Renderer object
        SDL_Renderer* getRenderer() const { return _theRenderer; }



        // Specify the properties of the object. (Moving, animated, collision, etc)
        virtual bool hasTexture() const override { return true; }



        // Perform the steps to call SDL_RenderCopy, etc
        virtual void render( const Camera& ) = 0;

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_DRAWABLE_H_


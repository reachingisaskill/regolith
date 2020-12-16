
#ifndef REGOLITH_ARCHITECTURE_TEXTURE_H_
#define REGOLITH_ARCHITECTURE_TEXTURE_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////
  // Forward declaration
  class Camera;
  class DataHandler;


////////////////////////////////////////////////////////////////////////////////
  // Base texture class.

  /*
   * This defines  one of the basic in-game assets available to use.
   *
   * The interface it defines must be fulfilled for the object to be renderable.
   * Inteded use is that game objects will own one or more of the derived classes and 
   * return a reference to this base class to the camera for something to be rendered.
   */
  class Texture
  {
    // Allow the camera special access for rendering
    friend class Camera;

////////////////////////////////////////////////////////////////////////////////
      // Private member variables
    private:


////////////////////////////////////////////////////////////////////////////////
      // Functions required to make this object render-able
    protected:

      // Return a pointer the raw, SDL texture
      virtual SDL_Texture* getSDLTexture() = 0;

      // Return a pointer to the clip rect for the rendering process
      virtual SDL_Rect* getClip() = 0;

      // Return the flip flag
      virtual SDL_RendererFlip getRendererFlip() = 0;

      // Return a pointer to the center point (If one is defined)
      virtual SDL_Point* getTextureCenter() = 0;

      // Return the rotation value
      virtual double getRotation() = 0;


      // Return a pointer to the surface to render
      virtual SDL_Surface* getUpdateSurface() = 0;

      // Set the newly rendered texture
      virtual void setRenderedTexture( SDL_Texture* ) = 0;


      // Clear the rendered texture
      virtual void clearSDLTexture() = 0;


////////////////////////////////////////////////////////////////////////////////
      // Public member functions
    public:
      // Trivial constructor
      Texture() {}

      // Virtual Destructor
      virtual ~Texture() {}

      // Force the derived classes to configure themselves
      virtual void configure( Json::Value&, DataHandler& ) = 0;

      // Return true if a surface needs to be rendered during the rendering cycle
      virtual bool update() const = 0;
  };
}

#endif // REGOLITH_ARCHITECTURE_TEXTURE_H_


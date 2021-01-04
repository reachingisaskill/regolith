
#ifndef REGOLITH_TEXTURES_PRIMITIVES_H_
#define REGOLITH_TEXTURES_PRIMITIVES_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Textures/Texture.h"

#include <iostream>
#include <string>


namespace Regolith
{

  class Primitive : public Texture
  {
    private:
      // Flag to signal the text has changed and must be re-rendered
      bool _update;

      SDL_Texture* _theTexture;
      SDL_Surface* _theSurface;
      SDL_Rect _clip;
      SDL_RendererFlip _flipFlag;
      SDL_Point _center;
      float _rotation;

      // Post-render options
      SDL_BlendMode _blendMod;
      Uint8 _alphaMod;
      SDL_Color _colourMod;


    protected:
////////////////////////////////////////////////////////////////////////////////
      // Functions required to make this object render-able

      // Return a pointer the raw, SDL texture
      virtual SDL_Texture* getSDLTexture() { return _theTexture; }

      // Return a pointer to the clip rect for the rendering process
      virtual SDL_Rect* getClip() { return &_clip; }

      // Return the flip flag
      virtual SDL_RendererFlip getRendererFlip() { return _flipFlag; }

      // Return a pointer to the center point (If one is defined)
      virtual SDL_Point* getTextureCenter() { return &_center; }

      // Return the rotation value
      virtual double getRotation() { return _rotation; }


      // Return a pointer to the surface to render
      virtual SDL_Surface* getUpdateSurface() { return _theSurface; }

      // Set the newly rendered texture
      virtual void setRenderedTexture( SDL_Texture* t );

      // Clear the rendered texture
      virtual void clearSDLTexture();


////////////////////////////////////////////////////////////////////////////////
    public:
      Primitive();

      virtual ~Primitive();

      // Return true if a surface needs to be rendered
      virtual bool update() const { return _update; }


      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( Json::Value&, DataHandler& );

      // Accessors
      virtual float getWidth() const override { return _clip.w; } 
      virtual float getHeight() const override { return _clip.h; }

      // Set the alpha value of the texture
      void setAlpha( Uint8 );
  };
}

#endif // REGOLITH_TEXTURES_SPRITESHEET_H_


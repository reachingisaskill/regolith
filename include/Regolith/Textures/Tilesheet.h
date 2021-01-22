
#ifndef REGOLITH_TEXTURES_TILESHEET_H_
#define REGOLITH_TEXTURES_TILESHEET_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Textures/Texture.h"
#include "Regolith/Assets/RawTexture.h"

#include <iostream>
#include <string>


namespace Regolith
{

  class Tilesheet : public Texture
  {
    private:
      // Pointer to the raw sdl texture
      mutable RawTexture* _rawTexture;
      SDL_Surface* _tiledSurface;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;
      double _rotation;

      // If its a spritesheet
      unsigned int _currentSprite;

    protected:
////////////////////////////////////////////////////////////////////////////////
      // Functions required to make this object render-able

      // Return a pointer the raw, SDL texture
      virtual SDL_Texture* getSDLTexture() { return _rawTexture->sdl_texture; }

      // Return a pointer to the clip rect for the rendering process
      virtual SDL_Rect* getClip() { return &_clip; }

      // Return the flip flag
      virtual SDL_RendererFlip getRendererFlip() { return _flipFlag; }

      // Return the rotation value
      virtual double getRotation() { return _rotation; }


      // Return a pointer to the surface to render
      virtual SDL_Surface* getUpdateSurface() { return _tiledSurface; }

      // Set the newly rendered texture
      virtual void setRenderedTexture( SDL_Texture* t );

      // Clear the rendered texture
      virtual void clearSDLTexture();


////////////////////////////////////////////////////////////////////////////////
    public:
      Tilesheet();

      virtual ~Tilesheet();

      // Return true if a surface needs to be rendered
      virtual bool update() const { return ( _rawTexture != nullptr ) && ( _rawTexture->sdl_texture == nullptr ); }


      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( Json::Value&, DataHandler& );


      // Accessors
      virtual float getWidth() const override { return _clip.w; } 
      virtual float getHeight() const override { return _clip.h; }

  };
}

#endif // REGOLITH_TEXTURES_TILESHEET_H_



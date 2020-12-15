
#ifndef REGOLITH_TEXTURES_SPRITESHEET_H_
#define REGOLITH_TEXTURES_SPRITESHEET_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Texture.h"
#include "Regolith/Assets/RawTexture.h"

#include <iostream>
#include <string>


namespace Regolith
{

  class Spritesheet : public Texture
  {
    private:
      // Pointer to the raw sdl texture
      mutable RawTexture* _rawTexture;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;
      SDL_Point _centerOffset;
      SDL_Point _center;
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

      // Return a pointer to the center point (If one is defined)
      virtual SDL_Point* getTextureCenter() { return &_center; }

      // Return the rotation value
      virtual double getRotation() { return _rotation; }


      // Return a pointer to the surface to render
      virtual SDL_Surface* getUpdateSurface() { return _rawTexture->surface; }

      // Set the newly rendered texture
      virtual void setRenderedTexture( SDL_Texture* t );

      // Clear the rendered texture
      virtual void clearSDLTexture();


////////////////////////////////////////////////////////////////////////////////
    public:
      Spritesheet();

      virtual ~Spritesheet();

      // Return true if a surface needs to be rendered during the rendering cycle
      virtual bool update() const { return ( _rawTexture != nullptr ) && ( _rawTexture->sdl_texture == nullptr ); }


      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( Json::Value&, DataHandler& );


      // Accessors
      int getWidth() const { return _clip.w; }
      int getHeight() const { return _clip.h; }


      // Update which cell of the sprite sheet should be displayed
      void setFrameNumber( unsigned int );

      // Sprite details
      int getNumberSprites() const { return _rawTexture->cells; }
      const unsigned int& currentSpriteNumber() { return _currentSprite; }
  };
}

#endif // REGOLITH_TEXTURES_SPRITESHEET_H_


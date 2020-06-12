
#ifndef REGOLITH_GAMEPLAY_TEXTURE_H_
#define REGOLITH_GAMEPLAY_TEXTURE_H_

#include "Regolith/Global/Global.h"

#include <iostream>
#include <string>


namespace Regolith
{
  // Forward declaration
  class Camera;
  class DataHandler;
  class RawTextureDetail;
  class RawStringDetail;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Raw Texture Structure (Because SDL_Texture's don't store their dimensions).

  struct RawTexture
  {
    SDL_Texture* texture;
    int width;
    int height;
    unsigned short int rows;
    unsigned short int columns;
    unsigned short cells;

    RawTexture() : texture( nullptr ), width( 0 ), height( 0 ), rows( 0 ), columns( 0 ), cells( 0 ) {}
    RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c ) : texture( t ), width( w ), height( h ), rows( r ), columns( c ), cells( r*c ) {}
    RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c, unsigned short n ) : texture( t ), width( w ), height( h ), rows( r ), columns( c ), cells( n ) {}
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful functions

  RawTexture makeTextureFromText( const RawStringDetail& );
  RawTexture makeTextureFromText( TTF_Font*, std::string, SDL_Color );

  RawTexture makeTextureFromFile( const RawTextureDetail& );
  RawTexture makeTextureFromFile( std::string, unsigned short r = 1, unsigned short c = 1 );
  RawTexture makeTextureFromFile( std::string, SDL_Color, unsigned short r = 1, unsigned short c = 1);

//  RawTexture makeTexture( Json::Value& );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture class. The simplest object of renderable data.

  class Texture
  {
    private:
      // Pointer to the raw sdl texture
      RawTexture* _theTexture;
      float _angle;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;

      // If its a spritesheet
      int _currentSprite;

      // If its animated
      float _updatePeriod;
      float _count;

    protected:
      // Return a reference to the clip object
      SDL_Rect& clip() { return _clip; }

      // Manually set the clip around the texture (not compatible with spritesheet/animations)
      virtual void setClip( SDL_Rect c );

    public:
      Texture();

      virtual ~Texture();


      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( Json::Value&, DataHandler& );

      // Returns true if the spritesheeet is to be updated every frame
      bool isAnimated() const { return _updatePeriod > 0u; }


      // Render with the current renderer object
      virtual void draw( SDL_Rect* );


      // Accessors
      int getWidth() const { return _clip.w; }
      int getHeight() const { return _clip.h; }
      virtual float getAngle() const { return _angle; }


      SDL_Rect getClip() { return _clip; }


      // Colour Modulation
      void setColor( Uint8, Uint8, Uint8 );

      // Modulate the opacity
      void setAlpha( Uint8 );

      // Set the blend mode
      void setBlendMode( SDL_BlendMode );

      // Set the transformation info
      void setRotation( double );

      // Flip the image
      void setFlip( SDL_RendererFlip );


      // Animation update period
      void setUpdatePeriod( float );

      // Update with time in ms
      virtual void update( float );


      // Sprite details
      int getNumberSprites() const { return _theTexture->cells; }
      void setSpriteNumber( int );
      const int& currentSpriteNumber() { return _currentSprite; }
  };
}

#endif // REGOLITH_GAMEPLAY_TEXTURE_H_


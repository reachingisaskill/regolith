
#include "Regolith/Assets/RawTexture.h"


namespace Regolith
{

  RawTexture::RawTexture() :
    sdl_texture( nullptr ),
    surface( nullptr ),
    width( 0 ),
    height( 0 ),
    rows( 0 ),
    columns( 0 ),
    cells( 0 ),
    update( false )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c ) :
    sdl_texture( t ),
    surface( nullptr ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( r*c ),
    update( false )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c, unsigned short n ) :
    sdl_texture( t ),
    surface( nullptr ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( n ),
    update( false )
  {
  }


  RawTexture::~RawTexture()
  {
    if ( surface != nullptr )
    {
      SDL_FreeSurface( surface );
    }
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _rawTexture->sdl_texture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _rawTexture->sdl_texture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _rawTexture->sdl_texture, blendmode );
  }



////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture creation functions

  SDL_Surface* loadSurfaceFromString( std::string textureString, TTF_Font* font, const SDL_Color& color )
  {
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureString.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "loadSurfaceFromString()", "Could not render text", true );
      ex.addDetail( "Text string", textureString );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    return textSurface;
  }


  SDL_Surface* loadSurfaceFromFile( std::string path, const SDL_Color& key )
  {
    // Load the image into a surface
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if ( loadedSurface == nullptr )
    {
      Exception ex( "loadSurfaceFromFile()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    if ( key.a != 0 )
    {
      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key.r, key.g, key.b ) );
    }

    return loadedSurface;
  }

}


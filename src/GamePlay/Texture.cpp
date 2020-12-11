
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/RawObjectDetails.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <cmath>


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////  
  // RawTexture Member function definitions

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


////////////////////////////////////////////////////////////////////////////////////////////////////  
  // Texture Member function definitions

  Texture::Texture() :
    _rawTexture( nullptr ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } ),
    _currentSprite( 0 )
  {
  }


  Texture::~Texture()
  {
  }


  void Texture::setNewSurface( SDL_Surface* surface )
  {
    if ( _rawTexture == nullptr )
    {
      SDL_FreeSurface( surface );
    }
    else if ( _rawTexture->surface != nullptr )
    {
      SDL_FreeSurface( _rawTexture->surface );
      _rawTexture->surface = surface;
    }
    else
    {
      _rawTexture->surface = surface;
    }
  }


  void Texture::setClip( SDL_Rect c )
  {
    _clip = c;
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


  void Texture::setFlip( SDL_RendererFlip flip )
  {
    _flipFlag = flip;
  }


  void Texture::flipVertical()
  {
    _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_VERTICAL);
  }


  void Texture::flipHorizontal()
  {
    _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_HORIZONTAL);
  }


  void Texture::setFrameNumber( unsigned int num )
  {
    _currentSprite = num;
    _clip.x = (_currentSprite % _rawTexture->columns) * _clip.w;
    _clip.y = (_currentSprite / _rawTexture->columns) * _clip.h;

    DEBUG_STREAM << "Texture::setFrameNumber : " << _currentSprite << " : " << _clip.x << ", " << _clip.y << ", " << _clip.w << ", " << _clip.h;
  }


  void Texture::configure( Json::Value& json_data, DataHandler& handler )
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    _rawTexture = handler.getRawTexture( texture_name );
    DEBUG_STREAM << "Found texture: " << texture_name << " : " << _rawTexture;

    _clip.x = 0;
    _clip.y = 0;
    _clip.w = _rawTexture->width / _rawTexture->columns;
    _clip.h = _rawTexture->height / _rawTexture->rows;

    if ( Utilities::validateJson( json_data, "start_number", Utilities::JSON_TYPE_INTEGER, false ) )
    {
      setFrameNumber( json_data["start_number"].asInt() );
    }
    else
    {
      setFrameNumber( 0 );
    }

    DEBUG_STREAM << "Texture::configure : " << _rawTexture->rows << "x" << _rawTexture->columns << " -> " << _rawTexture->cells << " start: " << _currentSprite;
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


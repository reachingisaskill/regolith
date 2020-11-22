
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
    texture( nullptr ),
    width( 0 ),
    height( 0 ),
    rows( 0 ),
    columns( 0 ),
    cells( 0 )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c ) :
    texture( t ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( r*c )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c, unsigned short n ) :
    texture( t ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( n )
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////  
  // Texture Member function definitions

  Texture::Texture() :
    _theTexture( nullptr ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } ),
    _currentSprite( 0 )
  {
  }


  Texture::~Texture()
  {
  }


//  void Texture::draw( SDL_Renderer* renderer, SDL_Rect* destination ) const
//  {
//    // Render it to the window
//    SDL_RenderCopyEx( renderer, _theTexture->texture, &_clip, destination, _angle, nullptr, _flipFlag );
//  }


  void Texture::setClip( SDL_Rect c )
  {
    _clip = c;
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _theTexture->texture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _theTexture->texture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _theTexture->texture, blendmode );
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
    int sprite_x = (_currentSprite % _theTexture->columns) * _clip.w;
    int sprite_y = (_currentSprite / _theTexture->columns) * _clip.h;

    _clip.x = sprite_x;
    _clip.y = sprite_y;
//    _clip.w = _spriteWidth;
//    _clip.h = _spriteHeight;
  }


  void Texture::configure( Json::Value& json_data, DataHandler& handler )
  {
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    _theTexture = handler.getRawTexture( texture_name );
    DEBUG_STREAM << "Found texture: " << texture_name << " : " << _theTexture;

    if ( json_data.isMember( "start_number" ) )
    {
      setFrameNumber( json_data["start_number"].asInt() );
    }
    else
    {
      setFrameNumber( 0 );
    }

    _clip.x = 0;
    _clip.y = 0;
    _clip.w = _theTexture->width / _theTexture->columns;
    _clip.h = _theTexture->height / _theTexture->rows;

    DEBUG_STREAM << "Texture::configure : " << _theTexture->rows << "x" << _theTexture->columns << " -> " << _theTexture->cells << " start: " << _currentSprite;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture creation functions

  SDL_Surface* loadSurfaceFromString( std::string textureString, TTF_Font* font, const SDL_Color& color )
  {
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureString.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "Scene::_addTextureFromText()", "Could not render text", true );
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
      Exception ex( "Scene::addTextureFromFile()", "Could not load image data", false );
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


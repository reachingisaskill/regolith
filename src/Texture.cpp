
#include "Texture.h"

#include "Exception.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <utility>


namespace Regolith
{

  Texture::Texture() :
    _theTexture( nullptr ),
    _width( 0 ),
    _height( 0 ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _theRenderer( nullptr )
  {
  }


  Texture::Texture( Texture&& other ) :
    _theTexture( std::exchange( other._theTexture, nullptr ) ),
    _width( std::move( other._width ) ),
    _height( std::move( other._height ) ),
    _angle( std::move( other._angle ) ),
    _flipFlag( std::move( other._flipFlag ) ),
    _theRenderer( std::move( other._theRenderer ) )
  {
  }


  Texture& Texture::operator=( Texture&& other ) 
  {
    // Clear the class first
    this->free();

    // Move the data memebers
    _theTexture =  std::exchange( other._theTexture, nullptr );
    _width = std::move( other._width );
    _height = std::move( other._height );
    _angle = std::move( other._angle );
    _flipFlag = std::move( other._flipFlag );
    _theRenderer = std::move( other._theRenderer );

    return *this;
  }


  Texture::~Texture()
  {
    // Delete all allocated memory
    this->free();
  }


  void Texture::free()
  {
    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
      _width = 0;
      _height = 0;
    }
  }


  void Texture::loadFromFile( std::string path, SDL_Color* key )
  {
    // Remove existing texture data
    this->free();

    // Load the image into a surface
    SDL_Texture* loadedTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

    if ( loadedSurface == nullptr )
    {
      Exception ex( "Texture::loadFromFile()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    if ( key != nullptr )
    {
      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key->r, key->g, key->b ) );
    }

    loadedTexture = SDL_CreateTextureFromSurface( _theRenderer, loadedSurface );

    if ( loadedTexture == nullptr )
    {
      SDL_FreeSurface( loadedSurface );
      Exception ex( "Texture::loadFromFile()", "Could not convert to texture", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    _theTexture = loadedTexture;
    _width = loadedSurface->w;
    _height = loadedSurface->h;

    SDL_FreeSurface( loadedSurface );
  }


  void Texture::loadFromText( std::string textureString, TTF_Font* font, SDL_Color color )
  {
    // Clear existing data
    this->free();

    // Render the text as TTF
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureString.c_str(), color );
    if ( textSurface == nullptr )
    {
      Exception ex( "Texture::loadFromText()", "Could not render text", false );
      ex.addDetail( "Text string", textureString );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    // Create the texture from the surface
    _theTexture = SDL_CreateTextureFromSurface( _theRenderer, textSurface );
    if ( _theTexture == nullptr )
    {
      // Remove before we throw
      SDL_FreeSurface( textSurface );
      // Throw the exception
      Exception ex( "Texture::loadFromText()", "Could not convert to texture", false );
      ex.addDetail( "Text string", textureString );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    _width = textSurface->w;
    _height = textSurface->h;

    // Remove the unneeded surface
    SDL_FreeSurface( textSurface );
  }


  void Texture::render( int x, int y, SDL_Rect* clip )
  {
    // Specify dimensions & location
    SDL_Rect renderRect = { x, y, _width, _height };

    if ( clip != nullptr )
    {
      renderRect.w = clip->w;
      renderRect.h = clip->h;
    }

    // Render it to the window
    SDL_RenderCopyEx( _theRenderer, _theTexture, clip, &renderRect, _angle, nullptr, _flipFlag );
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _theTexture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _theTexture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _theTexture, blendmode );
  }


  void Texture::setRotation( double angle )
  {
    _angle = angle;
  }


  void Texture::setFlip( SDL_RendererFlip flip )
  {
    _flipFlag = flip;
  }

}


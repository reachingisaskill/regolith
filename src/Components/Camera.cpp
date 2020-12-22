
#include "Regolith/Components/Camera.h"

#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/Textures/Texture.h"
#include "Regolith/Assets/RawTexture.h"
#include "Regolith/Components/Window.h"


namespace Regolith
{

  Camera::Camera( Window& window, SDL_Renderer* renderer, const int& width, const int& height, const float& scalex, const float& scaley ) :
    _theWindow( window ),
    _theRenderer( renderer ),
    _width( width ),
    _height( height ),
    _scaleX( scalex ),
    _scaleY( scaley ),
    _targetRect( {0, 0, 0, 0} )
  {
  }


  void Camera::resetRender() const
  {
    SDL_SetRenderDrawColor( _theRenderer, 0, 0, 0, 255 );
    SDL_RenderClear( _theRenderer );
  }


  void Camera::draw() const
  {
    SDL_RenderPresent( _theRenderer );
  }


  void Camera::clear()
  {
    INFO_LOG( "Camera::clear : Destroying SDL_Renderer" );
    _theWindow.destroy();
    INFO_LOG( "Camera::clear : Rendering now disabled." );
  }


  void Camera::renderRawTexture( RawTexture* raw_texture )
  {
    // Create the texture
    raw_texture->sdl_texture = SDL_CreateTextureFromSurface( _theRenderer, raw_texture->surface );

    // Check that it worked
    if ( raw_texture->sdl_texture == nullptr )
    {
      Exception ex( "RawTexture::renderRawTexture", "Could not convert surface to texture" );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

//      SDL_SetTextureColorMod( _rawTexture->sdl_texture, raw_texture->colourMod.r, raw_texture->colourMod.g, raw_texture->colourMod.b );
//      SDL_SetTextureAlphaMod( _rawTexture->sdl_texture, raw_texture->alpha );
//      SDL_SetTextureBlendMode( _rawTexture->sdl_texture, raw_texture->blendmode );
  }


  void Camera::renderTexture( Texture& texture )
  {
    while ( texture.update() )
    {
      // Get the base surface
      SDL_Surface* temp_surface = texture.getUpdateSurface();

      // Create the texture
      SDL_Texture* temp_texture = SDL_CreateTextureFromSurface( _theRenderer, temp_surface );

      // Check that it worked
      if ( temp_texture == nullptr )
      {
        Exception ex( "RawTexture::renderTexture", "Could not convert surface to texture" );
        ex.addDetail( "SDL error", SDL_GetError() );
        throw ex;
      }

//      SDL_SetTextureColorMod( _rawTexture->sdl_texture, red, green, blue );
//      SDL_SetTextureAlphaMod( _rawTexture->sdl_texture, alpha );
//      SDL_SetTextureBlendMode( _rawTexture->sdl_texture, blendmode );

      // Update the texture with the new SDL_Texture
      texture.setRenderedTexture( temp_texture );
    }
  }


  void Camera::renderDrawableObject( DrawableObject* object, Vector& camera_position )
  {
    _targetRect.x = ( object->getPosition().x() - camera_position.x() ) * _scaleX;
    _targetRect.y = ( object->getPosition().y() - camera_position.y() ) * _scaleY;
    _targetRect.w = object->getWidth() * _scaleX;
    _targetRect.h = object->getHeight() * _scaleY;

    Texture& texture = object->getTexture();

    // If a new surface has been provided, re-render the texture
    if ( texture.update() )
    {
      renderTexture( texture );
    }

    // Render to the back bufer
    SDL_RenderCopyEx( _theRenderer, texture.getSDLTexture(), texture.getClip(), &_targetRect, object->getRotation()+texture.getRotation(), texture.getTextureCenter(), (SDL_RendererFlip) (object->getFlipFlag() ^ texture.getRendererFlip()) );
  }


  void Camera::clearTexture( Texture& texture )
  {
    texture.clearSDLTexture();
  }

}


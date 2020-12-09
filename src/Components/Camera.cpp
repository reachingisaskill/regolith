
#include "Regolith/Components/Camera.h"

#include "Regolith/GamePlay/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
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


  void Camera::renderRawTexture( RawTexture* texture )
  {
    // If it's already renderered
    if ( texture->sdl_texture != nullptr )
    {
      SDL_DestroyTexture( texture->sdl_texture );
    }

    // Create the texture
    texture->sdl_texture = SDL_CreateTextureFromSurface( _theRenderer, texture->surface );

    DEBUG_STREAM << "Camera::renderRawTexture : SDL_Texture @ " << texture->sdl_texture;

    // Check that it worked
    if ( texture->sdl_texture == nullptr )
    {
      SDL_FreeSurface( texture->surface );
      texture->surface = nullptr;
      Exception ex( "RawTexture::renderTexture", "Could not convert surface to texture" );
      ex.addDetail( "SDL error", SDL_GetError() );
      throw ex;
    }

    // Delete the surface data
    SDL_FreeSurface( texture->surface );
    texture->surface = nullptr;
  }


  void Camera::renderPhysicalObject( PhysicalObject* object, Vector& camera_position )
  {
    _targetRect.x = ( object->getPosition().x() - camera_position.x() ) * _scaleX;
    _targetRect.y = ( object->getPosition().y() - camera_position.y() ) * _scaleY;
    _targetRect.w = object->getWidth() * _scaleX;
    _targetRect.h = object->getHeight() * _scaleY;

    const Texture& texture = object->getTexture();

    // If a new surface has been provided, re-render the texture
    if ( texture._rawTexture->surface != nullptr )
    {
      renderRawTexture( texture._rawTexture );
    }

    // Render to the back bufer
    SDL_RenderCopyEx( _theRenderer, texture._rawTexture->sdl_texture, &texture._clip, &_targetRect, object->getRotation(), nullptr, texture._flipFlag );
  }

}


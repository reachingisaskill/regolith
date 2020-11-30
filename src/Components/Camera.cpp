
#include "Regolith/Components/Camera.h"

#include "Regolith/Architecture/PhysicalObject.h"
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


  void Camera::renderRawTexture( RawTexture* texture ) const
  {
    // If it's already renderered
    if ( texture->texture != nullptr )
    {
      return;
    }

    // Create the texture
    texture->texture = SDL_CreateTextureFromSurface( _theRenderer, texture->surface );

    DEBUG_STREAM << "Camera::renderRawTexture : SDL_Texture @ " << texture->texture;

    // Check that it worked
    if ( texture->texture == nullptr )
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


  void Camera::renderPhysicalObject( PhysicalObject* object, Vector& camera_position ) const
  {
    _targetRect.x = ( object->getPosition().x() - camera_position.x() ) * _scaleX;
    _targetRect.y = ( object->getPosition().y() - camera_position.y() ) * _scaleY;
    _targetRect.w = object->getWidth() * _scaleX;
    _targetRect.h = object->getHeight() * _scaleY;

    DEBUG_STREAM << "Camera::renderPhysicalObject : Physical Object @ " << object;
    DEBUG_STREAM << "Camera::renderPhysicalObject : Position: " << _targetRect.x << ", " << _targetRect.y << " | " << _targetRect.w << ", " << _targetRect.h;
    DEBUG_STREAM << "Camera::renderPhysicalObject : Scales: " << _scaleX << ", " << _scaleY;

    const Texture& texture = object->getTexture();

    DEBUG_STREAM << "Camera::renderPhysicalObject : SDL_Texture @ " << texture._theTexture->texture;
    DEBUG_STREAM << "Camera::renderPhysicalObject : Clip : " << texture._clip.x << ", " << texture._clip.y << ", " << texture._clip.w << ", " << texture._clip.h;
    SDL_RenderCopyEx( _theRenderer, texture._theTexture->texture, &texture._clip, &_targetRect, object->getRotation(), nullptr, texture._flipFlag );
  }

}


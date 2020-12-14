
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


  void Camera::renderTexture( Texture& texture )
  {
    while ( texture.update() )
    {
      // Get the base surface
      SDL_Surface* temp_surface = texture.getUpdateSurface();

      // Create the texture
      SDL_Texture temp_texture = SDL_CreateTextureFromSurface( _theRenderer, temp_surface );
      DEBUG_STREAM << "Camera::renderRawTexture : SDL_Texture @ " << temp_texture;

      // Check that it worked
      if ( temp_texture == nullptr )
      {
        Exception ex( "RawTexture::renderTexture", "Could not convert surface to texture" );
        ex.addDetail( "SDL error", SDL_GetError() );
        throw ex;
      }

      // Update the texture with the new SDL_Texture
      texture.setRenderedTexture( temp_texture );
    }
  }


  void Camera::renderPhysicalObject( PhysicalObject* object, Vector& camera_position )
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
    SDL_RenderCopyEx( _theRenderer, texture.getSDLTexture(), texture.getClip(), &_targetRect, object->getRotation()+texture.getRotation(), texture.getTextureCenter(), (SDL_RendererFlip) other->getFlipFlag() ^ texture.getRendererFlip() );
  }

}


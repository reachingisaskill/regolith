
#include "Regolith/Components/Camera.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Window.h"


namespace Regolith
{

  Camera::Camera( SDL_Renderer*& renderer, const int& width, const int& height, const float& scalex, const float& scaley ) :
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
    SDL_DestroyRenderer( _theRenderer );
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

    const Texture& texture = object->getTexture();

    SDL_RenderCopyEx( _theRenderer, texture._theTexture->texture, &texture._clip, &_targetRect, object->getRotation(), nullptr, texture._flipFlag );
  }


//  SDL_Rect Camera::place( const SDL_Rect& rect ) const
//  {
//    SDL_Rect newRect;
////    newRect.x = (rect.x - _layerPosition.x()) * _scaleX;
////    newRect.y = (rect.y - _layerPosition.y()) * _scaleY;
////    newRect.w = rect.w * _scaleX;
////    newRect.h = rect.h * _scaleY;
//    return newRect;
//  }

}


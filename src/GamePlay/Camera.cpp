
#include "Regolith/GamePlay/Camera.h"
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
    _scaleY( scaley )
  {
  }


  void Camera::resetRender() const
  {
    SDL_SetRenderDrawColor( _theRenderer, defaultColour.r, defaultColour.g, defaultColour.b, defaultColour.a );
    SDL_RenderClear( _theRenderer );
  }


  void Camera::draw() const
  {
    SDL_RenderPresent( renderer );
  }


  void Camera::clear()
  {
    SDL_DestroyRenderer( _theRenderer );
  }


  void Camera::renderRawTexture( RawTexture* texture )
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



  SDL_Rect Camera::place( const SDL_Rect& rect ) const
  {
    SDL_Rect newRect;
    newRect.x = (rect.x - _layerPosition.x()) * _scaleX;
    newRect.y = (rect.y - _layerPosition.y()) * _scaleY;
    newRect.w = rect.w * _scaleX;
    newRect.h = rect.h * _scaleY;
    return newRect;
  }

}


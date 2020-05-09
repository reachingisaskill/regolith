
#include "Components/Camera.h"

#include "Architecture/PhysicalObject.h"
#include "Components/Window.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _sceneWidth( 0 ),
    _sceneHeight( 0 ),
    _limitX( 0 ),
    _limitY( 0 ),
    _x( 0 ),
    _y( 0 ),
    _zoom( 1.0 ),
    _theObject( nullptr ),
    _offsetX( 0 ),
    _offsetY( 0 )
  {
  }


  void Camera::configure( int scene_width, int scene_height )
  {
    _sceneWidth = scene_width;
    _sceneHeight = scene_height;
    INFO_STREAM << "Camera::configure() : ScenePlatformer Dims: " << _sceneWidth << ", " << _sceneHeight << ", Camera Dims: " << _windowWidth << ", " << _windowHeight << ", Limits: " << _limitX << ", " << _limitY << ", Pos: " << _x << ", " << _y;
  }


  void Camera::setPosition( Vector pos )
  {
    _x = pos.x();
    _y = pos.y();

    checkPosition();
  }


  void Camera::checkPosition()
  {
    if ( _x > _limitX ) _x = _limitX;
    else if ( _x < 0 ) _x = 0;

    if ( _y > _limitY ) _y = _limitY;
    else if ( _y < 0 ) _y = 0;
  }


  SDL_Rect Camera::place( const SDL_Rect& rect )
  {
    SDL_Rect newRect;
    newRect.x = (rect.x - _x) * Window::renderScaleX();
    newRect.y = (rect.y - _y) * Window::renderScaleY();
    newRect.w = rect.w * Window::renderScaleX();
    newRect.h = rect.h * Window::renderScaleY();
    return newRect;
  }


  void Camera::update( Uint32 time )
  {
    // If there's nothing to follow, don't move
    if ( _theObject == nullptr ) return;

    // Update the current position
    _x = _theObject->getPosition().x() - _offsetX;
    _y = _theObject->getPosition().y() - _offsetY;

    // Validate
    this->checkPosition();
  }


  void Camera::followMe( PhysicalObject* object )
  {
    _theObject = object;
    _offsetX = 0.5*object->getWidth() + 0.5*getWidth();
    _offsetY = 0.5*object->getHeight() + 0.5*getHeight();
  }

}


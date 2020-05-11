
#include "Regolith/GamePlay/Camera.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Window.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _layerWidth( 0 ),
    _layerHeight( 0 ),
    _limitX( 0 ),
    _limitY( 0 ),
    _x( 0 ),
    _y( 0 ),
    _zoom( 1.0 ),
    _theObject( nullptr ),
    _offsetX( 0 ),
    _offsetY( 0 ),
    _scaleX( Manager::getInstance()->getWindow().renderScaleX() ),
    _scaleY( Manager::getInstance()->getWindow().renderScaleY() )
  {
  }


  void Camera::configure( int scene_width, int scene_height )
  {
    _layerWidth = scene_width;
    _layerHeight = scene_height;

    _limitX = scene_width - Manager::getInstance()->getWindow().getResolutionWidth();
    _limitY = scene_height - Manager::getInstance()->getWindow().getResolutionHeight();

    INFO_STREAM << "Camera configured. Dims: " << _layerWidth << ", " << _layerHeight << ", Limits: " << _limitX << ", " << _limitY << ", Pos: " << _x << ", " << _y;
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


  SDL_Rect Camera::place( const SDL_Rect& rect ) const
  {
    SDL_Rect newRect;
    newRect.x = (rect.x - _x) * _scaleX;
    newRect.y = (rect.y - _y) * _scaleY;
    newRect.w = rect.w * _scaleX;
    newRect.h = rect.h * _scaleY;
    return newRect;
  }


  void Camera::update( float )
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


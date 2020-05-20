
#include "Regolith/GamePlay/Camera.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Window.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _position( 0.0 ),
    _layerWidth( 0 ),
    _layerHeight( 0 ),
    _limit( 0.0 ),
    _theObject( nullptr ),
    _offset( 0.0 ),
    _velocityScale( 0.0 ),
    _scaleX( Manager::getInstance()->getWindow().renderScaleX() ),
    _scaleY( Manager::getInstance()->getWindow().renderScaleY() )
  {
  }


  void Camera::configure( int scene_width, int scene_height, Vector move_scale )
  {
    _layerWidth = scene_width;
    _layerHeight = scene_height;

    _limit = Vector( scene_width - Manager::getInstance()->getWindow().getResolutionWidth(),
                     scene_height - Manager::getInstance()->getWindow().getResolutionHeight() );

    _velocityScale = move_scale;

    INFO_STREAM << "Camera configured. Dims: " << _layerWidth << ", " << _layerHeight << ", Limits: " << _limit  << ", Pos: " << _position << "Scaled movement by " << _velocityScale;
  }


  void Camera::setPosition( Vector pos )
  {
    _position = pos;
    checkPosition();
  }


  void Camera::checkPosition()
  {
    if ( _position.x() > _limit.x() ) _position.x() = _limit.x();
    else if ( _position.x() < 0 ) _position.x() = 0;

    if ( _position.y() > _limit.y() ) _position.y() = _limit.y();
    else if ( _position.y() < 0 ) _position.y() = 0;
  }


  SDL_Rect Camera::place( const SDL_Rect& rect ) const
  {
    SDL_Rect newRect;
    newRect.x = (rect.x - _position.x()) * _scaleX;
    newRect.y = (rect.y - _position.y()) * _scaleY;
    newRect.w = rect.w * _scaleX;
    newRect.h = rect.h * _scaleY;
    return newRect;
  }


  void Camera::update( float )
  {
    DEBUG_LOG( "Updating Camera" );
    // If there's nothing to follow, don't move
    if ( _theObject == nullptr ) return;

    DEBUG_STREAM << _velocityScale << ", " << _theObject->getPosition();
    // Update the current position
    _position = _velocityScale % ( _theObject->getPosition() - _offset );

    // Validate
    this->checkPosition();
  }


  void Camera::followMe( PhysicalObject* object )
  {
    _theObject = object;
    _offset.x() = 0.5*object->getWidth() + 0.5*getWidth();
    _offset.y() = 0.5*object->getHeight() + 0.5*getHeight();
  }

}


#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GamePlay/Camera.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Window.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _position( 0.0 ),
    _lowerLimit( 0.0 ),
    _upperLimit( 0.0 ),
    _theObject( nullptr ),
    _offset( 0.0 ),
    _scaleX( Manager::getInstance()->getWindow().renderScaleX() ),
    _scaleY( Manager::getInstance()->getWindow().renderScaleY() )
  {
  }


  void Camera::configure( Vector lower, Vector upper )
  {
    _lowerLimit = lower;
    _upperLimit = upper - Vector( Manager::getInstance()->getWindow().getResolutionWidth(),
                                  Manager::getInstance()->getWindow().getResolutionHeight() );

    INFO_STREAM << "Camera configured. Limits: " << _lowerLimit << " -> " << _upperLimit  << ", Pos: " << _position;
  }


  void Camera::setPosition( Vector pos )
  {
    _position = pos;
    checkPosition();
  }


  void Camera::checkPosition()
  {
    if ( _position.x() > _upperLimit.x() ) _position.x() = _upperLimit.x();
    else if ( _position.x() < _lowerLimit.x() ) _position.x() = _lowerLimit.x();

    if ( _position.y() > _upperLimit.y() ) _position.y() = _upperLimit.y();
    else if ( _position.y() < _lowerLimit.y() ) _position.y() = _lowerLimit.y();
  }


  void Camera::setLayer( ContextLayer* layer )
  {
    _layerPosition = layer->_position + (layer->_movementScale % _position);
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


  void Camera::update( float )
  {
    DEBUG_LOG( "Updating Camera" );
    // If there's nothing to follow, don't move
    if ( _theObject == nullptr ) return;

    // Update the current position
    _position =  _theObject->getPosition() - _offset;
    DEBUG_STREAM << "CAMERA : " << _theObject->getPosition()  << ", " << _position << ", " << _offset;

    // Validate
    this->checkPosition();
  }


  void Camera::followMe( PhysicalObject* object )
  {
    _theObject = object;
    _offset.x() = 0.5*Manager::getInstance()->getWindow().getResolutionWidth() - 0.5*object->getWidth();
    _offset.y() = 0.5*Manager::getInstance()->getWindow().getResolutionHeight() - 0.5*object->getHeight();
  }

}



#include "Regolith/GamePlay/Camera.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Window.h"


namespace Regolith
{

  Camera::Camera() :
    _position( 0.0 ),
    _theObject( nullptr ),
    _offset( 0.0 ),
    _scaleX( Manager::getInstance()->getWindow().renderScaleX() ),
    _scaleY( Manager::getInstance()->getWindow().renderScaleY() )
  {
  }


  void Camera::setPosition( Vector pos )
  {
    _position = pos;
  }


  void Camera::render( ContextLayer* layer )
  {
    Vector layerPosition = layer->_position + (layer->_movementScale % _position);



    // This is where the magic happens!



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
  }


  void Camera::followMe( PhysicalObject* object )
  {
    _theObject = object;
    _offset.x() = 0.5*Manager::getInstance()->getWindow().getResolutionWidth() - 0.5*object->getWidth();
    _offset.y() = 0.5*Manager::getInstance()->getWindow().getResolutionHeight() - 0.5*object->getHeight();
  }

}


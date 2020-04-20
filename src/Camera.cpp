
#include "Camera.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _sceneWidth( 0 ),
    _sceneHeight( 0 ),
    _width( 0 ),
    _height( 0 ),
    _limitX( 0 ),
    _limitY( 0 ),
    _x( 0 ),
    _y( 0 ),
    _scaleX( 1.0 ),
    _scaleY( 1.0 )
  {
  }


  Camera::Camera( int scene_width, int scene_height, int width, int height ) :
    _sceneWidth( scene_width ),
    _sceneHeight( scene_height ),
    _width( width ),
    _height( height ),
    _limitX( _sceneWidth - width ),
    _limitY( _sceneHeight - height ),
    _x( 0 ),
    _y( 0 ),
    _scaleX( 1.0 ),
    _scaleY( 1.0 )
  {
  }


  void Camera::configure( int scene_width, int scene_height, int width, int height )
  {
    _sceneWidth = scene_width;
    _sceneHeight = scene_height;
    this->setSize( width, height );
    INFO_STREAM << "Camera::configure() : Scene Dims: " << _sceneWidth << ", " << _sceneHeight << ", Camera Dims: " << _width << ", " << _height << ", Limits: " << _limitX << ", " << _limitY << ", Pos: " << _x << ", " << _y;
  }


  void Camera::setPosition( int x, int y )
  {
    if ( x > _limitX ) _x = _limitX;
    else if ( x < 0 ) _x = 0;
    else _x = x;

    if ( y > _limitY ) _y = _limitY;
    else if ( y < 0 ) _y = 0;
    else _y = y;
  }


  void Camera::setSize( int x, int y )
  {
    // Set the new camera size ( must fit the scene )
    _width = x;
    if ( _width > _sceneWidth ) _width = _sceneWidth;
    _height = y;
    if ( _height > _sceneHeight ) _height = _sceneHeight;

    // Set the new corresponding limits
    _limitX = _sceneWidth - _width;
    _limitY = _sceneHeight - _height;

    // Makesure the camera is still within the scene
    if ( _x > _limitX ) _x = _limitX;
    else if ( _x < 0 ) _x = 0;
    if ( _y > _limitY ) _y = _limitY;
    else if ( _y < 0 ) _y = 0;
  }


  void Camera::move( int dx, int dy )
  {
    _x += dx;
    if ( _x > _limitX ) _x = _limitX;
    else if ( _x < 0 ) _x = 0;

    _y += dy;
    if ( _y > _limitY ) _y = _limitY;
    else if ( _y < 0 ) _y = 0;
  }


  void Camera::updateScale( int window_w, int window_h )
  {
    _scaleX = window_w / _width;
    _scaleY = window_h / _height;
  }


  SDL_Rect Camera::place( const SDL_Rect& rect )
  {
    SDL_Rect newRect;
    newRect.x = rect.x - _x;
    newRect.y = rect.y - _y;
    newRect.w = rect.w * _scaleX;
    newRect.h = rect.h * _scaleY;
    return newRect;
  }

}


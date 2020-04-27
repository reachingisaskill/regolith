
#include "Camera.h"

#include "Drawable.h"
#include "InputHandler.h"

#include "logtastic.h"


namespace Regolith
{

  Camera::Camera() :
    _currentMode( CAMERA_FIXED ),
    _sceneWidth( 0 ),
    _sceneHeight( 0 ),
    _width( 0 ),
    _height( 0 ),
    _limitX( 0 ),
    _limitY( 0 ),
    _x( 0 ),
    _y( 0 ),
    _scaleX( 1.0 ),
    _scaleY( 1.0 ),
    _velocityX( 0 ),
    _velocityY( 0 ),
    _speed( 1 ),
    _theObject( nullptr ),
    _offsetX( 0 ),
    _offsetY( 0 )
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
    INFO_STREAM << "Camera::configure() : ScenePlatformer Dims: " << _sceneWidth << ", " << _sceneHeight << ", Camera Dims: " << _width << ", " << _height << ", Limits: " << _limitX << ", " << _limitY << ", Pos: " << _x << ", " << _y;
  }


  void Camera::setPosition( int x, int y )
  {
    _x = x;
    _y = y;

    checkPosition();
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
    checkPosition();
  }


  void Camera::move( int dx, int dy )
  {
    _x += dx;
    _y += dy;

    checkPosition();
  }


  void Camera::checkPosition()
  {
    if ( _x > _limitX ) _x = _limitX;
    else if ( _x < 0 ) _x = 0;

    if ( _y > _limitY ) _y = _limitY;
    else if ( _y < 0 ) _y = 0;
  }


  void Camera::updateScale( int window_w, int window_h )
  {
    _scaleX = (float)window_w / (float)_width;
    _scaleY = (float)window_h / (float)_height;
    DEBUG_STREAM << "New camera scale = " << _scaleX << ", " << _scaleY << " (" << window_w << ", " << window_h << ")";
  }


  SDL_Rect Camera::place( const SDL_Rect& rect )
  {
    SDL_Rect newRect;
    newRect.x = (rect.x - _x) * _scaleX;
    newRect.y = (rect.y - _y) * _scaleY;
    newRect.w = rect.w * _scaleX;
    newRect.h = rect.h * _scaleY;
    return newRect;
  }


  void Camera::registerEvents( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_UP );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_DOWN );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_LEFT );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_RIGHT );
  }


  void Camera::booleanAction( const InputAction& action, bool value )
  {
    if ( _currentMode == CAMERA_FLYING )
    {
      DEBUG_STREAM << " Camera : " << (int)action << " : " << value;
      switch ( action )
      {
        case INPUT_ACTION_MOVE_UP :
          if ( value ) _velocityY -= _speed;
          else _velocityY += _speed;
          break;

        case INPUT_ACTION_MOVE_DOWN :
          if ( value ) _velocityY += _speed;
          else _velocityY -= _speed;
          break;

        case INPUT_ACTION_MOVE_LEFT :
          if ( value ) _velocityX -= _speed;
          else _velocityX += _speed;
          break;

        case INPUT_ACTION_MOVE_RIGHT :
          if ( value ) _velocityX += _speed;
          else _velocityX -= _speed;
          break;

        default :
          break;
      }
    }
  }


  void Camera::update( Uint32 time )
  {
    switch ( _currentMode )
    {
      case CAMERA_FIXED :
        break;

      case CAMERA_CONSTANT_VELOCITY :
      case CAMERA_FLYING :
        this->x() += _velocityX * time;
        this->y() += _velocityY * time;
        break;

      case CAMERA_FOLLOWING :
        if ( _theObject == nullptr ) return;
        this->setPosition( _theObject->getPosition().x() - _offsetX, _theObject->getPosition().y() + _offsetY );

      default:
        break;
    }

    this->checkPosition();
  }


  void Camera::followMe( Drawable* object )
  {
    _theObject = object;
    _offsetX = 0.5*object->getWidth() + 0.5*getWidth();
    _offsetY = 0.5*object->getHeight() + 0.5*getHeight();
  }

}


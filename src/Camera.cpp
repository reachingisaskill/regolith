
#include "Camera.h"

#include "Drawable.h"
#include "InputManager.h"

#include "logtastic.h"


namespace Regolith
{

  float Camera::_scaleX = 1.0;
  float Camera::_scaleY = 1.0;

  int Camera::_windowWidth = 0;
  int Camera::_windowHeight = 0;

  Camera::Camera() :
    _currentMode( CAMERA_FIXED ),
    _sceneWidth( 0 ),
    _sceneHeight( 0 ),
    _limitX( 0 ),
    _limitY( 0 ),
    _x( 0 ),
    _y( 0 ),
    _velocityX( 0 ),
    _velocityY( 0 ),
    _speed( 1 ),
    _theObject( nullptr ),
    _offsetX( 0 ),
    _offsetY( 0 )
  {
  }


  Camera::Camera( int scene_width, int scene_height, int width, int height ) :
    _currentMode( CAMERA_FIXED ),
    _sceneWidth( scene_width ),
    _sceneHeight( scene_height ),
    _limitX( _sceneWidth - width ),
    _limitY( _sceneHeight - height ),
    _x( 0 ),
    _y( 0 ),
    _velocityX( 0 ),
    _velocityY( 0 ),
    _speed( 1 ),
    _theObject( nullptr ),
    _offsetX( 0 ),
    _offsetY( 0 )
  {
    _windowWidth = width;
    _windowHeight = height;
  }


  void Camera::configure( int scene_width, int scene_height, int width, int height )
  {
    _sceneWidth = scene_width;
    _sceneHeight = scene_height;
    this->setSize( width, height );
    INFO_STREAM << "Camera::configure() : ScenePlatformer Dims: " << _sceneWidth << ", " << _sceneHeight << ", Camera Dims: " << _windowWidth << ", " << _windowHeight << ", Limits: " << _limitX << ", " << _limitY << ", Pos: " << _x << ", " << _y;
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
    _windowWidth = x;
    if ( _windowWidth > _sceneWidth ) _windowWidth = _sceneWidth;
    _windowHeight = y;
    if ( _windowHeight > _sceneHeight ) _windowHeight = _sceneHeight;

    // Set the new corresponding limits
    _limitX = _sceneWidth - _windowWidth;
    _limitY = _sceneHeight - _windowHeight;

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
    _scaleX = (float)window_w / (float)_windowWidth;
    _scaleY = (float)window_h / (float)_windowHeight;
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


  void Camera::registerActions( InputHandler* handler )
  {
    INFO_LOG( "Registering camera actions with input handler" );
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
        break;

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


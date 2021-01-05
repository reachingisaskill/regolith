
#include "Regolith/Components/Window.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/InputManager.h"

#include <iostream>
#include <sstream>
#include <utility>


namespace Regolith
{

  Window::Window() :
    _exists( false ),
    _theWindow( nullptr ),
    _theRenderer( nullptr ),
    _title(),
    _defaultColour( { 0, 0, 0, 255 } ),
    _width( 0 ),
    _height( 0 ),
    _resolutionWidth( 0 ),
    _resolutionHeight( 0 ),
    _vsyncOn( true ),
    _mouseFocus( false ),
    _keyboardFocus( false ),
    _minimized( false ),
    _fullscreen( false ),
    _scaleX( 1.0 ),
    _scaleY( 1.0 ),
    _camera( *this, _theRenderer, _resolutionWidth, _resolutionHeight, _scaleX, _scaleY )
  {
  }


  Window::~Window()
  {
  }


  void Window::destroy()
  {
    SDL_DestroyRenderer( _theRenderer );
    SDL_DestroyWindow( _theWindow );

    _theRenderer = nullptr;
    _theWindow = nullptr;

    _exists = false;
  }


  void Window::configure( Json::Value& json_data )
  {
    validateJson( json_data, "screen_width", JsonType::INTEGER );
    validateJson( json_data, "screen_height", JsonType::INTEGER );
    validateJson( json_data, "title", JsonType::STRING );
    validateJson( json_data, "v-sync", JsonType::BOOLEAN );

    // Load the window configuration
    _width = json_data["screen_width"].asInt();
    _height = json_data["screen_height"].asInt();
    _resolutionWidth = _width;
    _resolutionHeight = _height;
    _title = json_data["title"].asString();
    _vsyncOn = json_data["v-sync"].asBool();


    // Set the default colour
    if( validateJson( json_data, "default_colour", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["default_colour"], 4, JsonType::INTEGER );

      Json::Value color = json_data["default_color"];
      _defaultColour.r = color[0].asInt();
      _defaultColour.g = color[1].asInt();
      _defaultColour.b = color[2].asInt();
      _defaultColour.a = color[3].asInt();
    }
  }


  Camera& Window::create()
  {
    if ( _exists )
    {
      Exception ex( "Window::create()", "Window already exists - only one thread may have access to the renderer." );
      throw ex;
    }
    _exists = true;

    _theWindow = SDL_CreateWindow( _title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( _theWindow == nullptr )
    {
      Exception ex( "Window::create()", "Could not create window" );
      ex.addDetail( "Window title", _title );
      ex.addDetail( "Window width", _width );
      ex.addDetail( "Window height", _height );
      throw ex;
    }

    if ( _vsyncOn )
    {
      _theRenderer =  SDL_CreateRenderer( _theWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    }
    else
    {
      _theRenderer =  SDL_CreateRenderer( _theWindow, -1, SDL_RENDERER_ACCELERATED );
    }

    if ( _theRenderer == nullptr )
    {
      Exception ex( "Window::create()", "Could not create SDL Renderer" );
      throw ex;
    }

    _camera.setRenderer( _theRenderer );

    return _camera;
  }


  void Window::toggleFullScreen()
  {
    if ( _fullscreen )
    {
      SDL_SetWindowFullscreen( _theWindow, SDL_FALSE );
      _fullscreen = false;
    }
    else
    {
      _width = _resolutionWidth;
      _height = _resolutionHeight;
      SDL_SetWindowSize( _theWindow, _width, _height );
      SDL_SetWindowFullscreen( _theWindow, SDL_TRUE );
      _fullscreen = true;
      _minimized = false;
    }
  }


  void Window::registerEvents( InputManager& manager )
  {
    manager.registerEventRequest( this, REGOLITH_EVENT_WINDOW );
  }


  void Window::setTitle( std::string title )
  {
    _title = title;
    SDL_SetWindowTitle( _theWindow, _title.c_str() );
  }

  void Window::eventAction( const RegolithEvent&, const SDL_Event& e  )
  {
    switch ( e.window.event )
    {
      case SDL_WINDOWEVENT_SIZE_CHANGED :
        _width = e.window.data1;
        _height = e.window.data2;
        _scaleX = (float)_width / (float)_resolutionWidth;
        _scaleY = (float)_height / (float)_resolutionHeight;
//        SDL_RenderPresent( _theRenderer );
        break;

      case SDL_WINDOWEVENT_EXPOSED :
//        SDL_RenderPresent( _theRenderer );
        break;

      case SDL_WINDOWEVENT_ENTER :
        _mouseFocus = true;
        break;

      case SDL_WINDOWEVENT_LEAVE :
        _mouseFocus = false;
        break;

      case SDL_WINDOWEVENT_FOCUS_GAINED :
        _keyboardFocus = true;
        break;

      case SDL_WINDOWEVENT_FOCUS_LOST :
        _keyboardFocus = false;
        break;

      case SDL_WINDOWEVENT_MINIMIZED :
        _minimized = true;
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_ENGINE_PAUSE );
        break;

      case SDL_WINDOWEVENT_MAXIMIZED :
        _minimized = false;
        break;

      case SDL_WINDOWEVENT_RESTORED :
        _minimized = false;
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_ENGINE_RESUME );
        break;
    }

#ifdef REGOLITH_VERSION_DEBUG
    std::stringstream text;
    text << "SDL Testing | Keyboard Focus: " << ( _keyboardFocus ? "On" : "Off" ) << ". Mouse Focus " << ( _mouseFocus ? "On" : "Off" ) << " : " << _width << "x" << _height << " SCALE: " << _scaleX << ", " << _scaleY;
    SDL_SetWindowTitle( _theWindow, text.str().c_str() );
#endif
  }

}


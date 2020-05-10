
#include "Components/Window.h"
#include "Components/Exception.h"
#include "Managers/Manager.h"
#include "Managers/InputManager.h"

#include <iostream>
#include <sstream>
#include <utility>


namespace Regolith
{

  Window::Window( std::string title ) :
    _theWindow( nullptr ),
    _title( title ),
    _width( 0 ),
    _height( 0 ),
    _mouseFocus( false ),
    _keyboardFocus( false ),
    _minimized( false ),
    _fullscreen( false ),
    _theRenderer( nullptr )
  {
  }

  Window::Window( Window&& win ) :
    _theWindow( std::exchange( win._theWindow, nullptr ) ),
    _title( std::move( win._title ) ),
    _width( std::move( win._width ) ),
    _height( std::move( win._height ) ),
    _mouseFocus( std::move( win._mouseFocus ) ),
    _keyboardFocus( std::move( win._keyboardFocus ) ),
    _minimized( std::move( win._minimized ) ),
    _fullscreen( std::move( win._fullscreen ) ),
    _theRenderer( std::move( win._theRenderer ) )
  {
  }


  Window& Window::operator=( Window&& win )
  {
    _theWindow = std::exchange( win._theWindow, nullptr );
    _title = std::move( win._title );
    _width = std::move( win._width );
    _height = std::move( win._height );
    _mouseFocus = std::move( win._mouseFocus );
    _keyboardFocus = std::move( win._keyboardFocus );
    _minimized = std::move( win._minimized );
    _fullscreen = std::move( win._fullscreen );
    _theRenderer = std::move( win._theRenderer );

    return *this;
  }


  SDL_Renderer* Window::init( int width, int height )
  {
    _theWindow = SDL_CreateWindow( _title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( _theWindow == nullptr )
    {
      Exception ex( "Window::init()", "Could not create window", false );
      ex.addDetail( "Window title", _title );
      throw ex;
    }

    _width = width;
    _height = height;

    return SDL_CreateRenderer( _theWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  }




  void Window::free()
  {
    SDL_DestroyWindow( _theWindow );
    _theWindow = nullptr;
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
      SDL_SetWindowFullscreen( _theWindow, SDL_TRUE );
      _fullscreen = true;
      _minimized = false;
    }
  }


  void Window::registerEvents( InputManager* manager )
  {
    manager->registerEventRequest( this, REGOLITH_EVENT_WINDOW );
  }


  void Window::eventAction( const RegolithEvent&, const SDL_Event& e  )
  {
    bool updateCaption = false;

    switch ( e.window.event )
    {
      case SDL_WINDOWEVENT_SIZE_CHANGED :
        _width = e.window.data1;
        _height = e.window.data2;
        updateCaption = true;
        SDL_RenderPresent( _theRenderer );
        break;

      case SDL_WINDOWEVENT_EXPOSED :
        SDL_RenderPresent( _theRenderer );
        break;

      case SDL_WINDOWEVENT_ENTER :
        _mouseFocus = true;
        updateCaption = true;
        break;

      case SDL_WINDOWEVENT_LEAVE :
        _mouseFocus = false;
        updateCaption = true;
        break;

      case SDL_WINDOWEVENT_FOCUS_GAINED :
        _keyboardFocus = true;
        updateCaption = true;
        break;

      case SDL_WINDOWEVENT_FOCUS_LOST :
        _keyboardFocus = false;
        updateCaption = true;
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_SCENE_PAUSE );
        break;

      case SDL_WINDOWEVENT_MINIMIZED :
        _minimized = true;
        updateCaption = true;
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_SCENE_PAUSE );
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_ENGINE_PAUSE );
        break;

      case SDL_WINDOWEVENT_MAXIMIZED :
        _minimized = false;
        updateCaption = true;
        break;

      case SDL_WINDOWEVENT_RESTORED :
        _minimized = false;
        updateCaption = true;
        Manager::getInstance()->raiseEvent( REGOLITH_EVENT_ENGINE_RESUME );
        break;
    }

    if ( updateCaption )
    {
      std::stringstream text;
      text << "SDL Testing | Keyboard Focus: " << ( _keyboardFocus ? "On" : "Off" ) << ". Mouse Focus " << ( _mouseFocus ? "On" : "Off" ) << " : " << _width << "x" << _height;

      SDL_SetWindowTitle( _theWindow, text.str().c_str() );

      Manager::getInstance()->raiseEvent( REGOLITH_EVENT_CAMERA_RESIZE );
    }
  }

}

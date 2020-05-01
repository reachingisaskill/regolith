
#ifndef REGOLITH_MANAGER_H_
#define REGOLITH_MANAGER_H_

#include "Definitions.h"

#include "Singleton.h"
#include "Window.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "Scene.h"
#include "ObjectBuilder.h"
#include "SceneBuilder.h"

#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Regolith
{
  // Forward declarations
  class Engine;
  class Context;

  // Useful typedefs
  typedef std::vector< Scene* > SceneList;
  typedef std::map< std::string, TTF_Font* > FontMap;
  typedef std::deque< Context* > ContextStack;


  // Manager class
  // Global storage for all scenes, renderers and windows.
  // Current version limited to a single window.
  class Manager : public Singleton< Manager >
  {
    friend class Singleton< Manager >; // Required to enable the singleton pattern

    private:
      Engine* _theEngine;
      Window* _theWindow;
      SDL_Renderer* _theRenderer;
      InputManager* _theInput;
      AudioManager* _theAudio;
      ContextStack _contexts;

      ObjectBuilder* _theBuilder;
      SceneBuilder* _theSceneBuilder;
      SceneList _scenes;
      FontMap _fonts;

      std::string _title;
      TTF_Font* _defaultFont;
      SDL_Color _defaultColor;

      Uint32 _eventStartIndex;
      SDL_Event _gameEvents[REGOLITH_EVENT_TOTAL];

      Vector _gravityConst;
      float _dragConst;


    protected:
      Manager();


      // Load the input device configuration
      void _loadInput( Json::Value& );

      // Load the input device configuration
      void _loadAudio( Json::Value& );

    public:
      virtual ~Manager();

      // Initialise the manager class from the configuration file
      void init( std::string );


      // Load the first scene into the engine and give it control
      void run();


      // Return a pointer to the texture builder
      ObjectBuilder* getObjectBuilder() { return _theBuilder; }

      // Return a pointer to the texture builder
      SceneBuilder* getSceneBuilder() { return _theSceneBuilder; }

      // Get the pointer to the window
      SDL_Renderer* getRendererPointer() { return _theRenderer; }

      // Get the pointer to the window
      Window* getWindowPointer() { return _theWindow; }

      // The engine being used
      Engine* getEnginePointer() { return _theEngine; }

      // Return a pointer to the required font data
      TTF_Font* getFontPointer( std::string );

      // Return a pointer to the input manager
      InputManager* getInputManager() { return _theInput; }

      // Return a pointer to the input manager
      AudioManager* getAudioManager() { return _theAudio; }



      // Return a pointer to the required scene
      Scene* getScene( size_t );

      // Return a pointer to the required scene
      Scene* loadScene( size_t );

      // Return the number of scenes
      size_t getNumberScenes() { return _scenes.size(); }



      // Return the default font
      TTF_Font* getDefaultFont() { return _defaultFont; }

      // Return the default colour
      SDL_Color getDefaultColour() { return _defaultColor; }



      // Configure the list of user events for game events
      void configureEvents();

      // Push a user event into the SDL event queue
      void raiseEvent( RegolithEvent );



      // Set the current context pointer
      void pushContext( Context* c ) { _contexts.push_front( c ); }

      // Set the current context pointer
      void popContext();

      // Get the current context
      ContextStack& contextStack() { return _contexts; }

      // Get the current context
      Context* currentContext() { return _contexts.front(); }



      // Return the constant g
      inline Vector getGravity() { return _gravityConst; }

      // Return the drag
      inline float getDrag() { return _dragConst; }
  };

}

#endif // REGOLITH_MANAGER_H_


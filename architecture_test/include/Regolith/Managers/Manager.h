
#ifndef REGOLITH_MANAGER_MANAGER_H_
#define REGOLITH_MANAGER_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Contexts/ContextFactory.h"
#include "Regolith/GameObjects/ObjectFactory.h"

#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Regolith
{
  // Forward declarations
  class Engine;
  class Context;
  class Window;

  // Useful typedefs
  typedef std::map< std::string, RawTexture > RawTextureMap;
  typedef std::vector< Scene* > SceneList;
  typedef std::map< std::string, TTF_Font* > FontMap;
  typedef std::deque< Context* > ContextStack;
  typedef std::map< std::string, unsigned int > TeamNameMap;


  // Manager class
  // Global storage for all scenes, renderers and windows.
  // Current version limited to a single window.
  class Manager : public Singleton< Manager >
  {
    friend class Singleton< Manager >; // Required to enable the singleton pattern

    private:
      // Crucial objects for Regolith
      Window _theWindow;
      InputManager _theInput;
      AudioManager _theAudio;
      Engine _theEngine;
      SDL_Renderer* _theRenderer;

      // Factories to provide object/context creation
      ObjectFactory _objectFactory;
      ContextFactory _contextFactory;

      // Owned memory for objects, contexts, etc
      FontMap _fonts;
      RawTextureMap _rawTextures;
      TeamNameMap _teamNames;
      NamedVector<Drawable, true> _objects;
      NamedVector<Context, true> _contexts;

      // Useful global constants
      std::string _title;
      TTF_Font* _defaultFont;
      SDL_Color _defaultColor;

      // Info for SDL user events
      Uint32 _eventStartIndex;
      SDL_Event _gameEvents[REGOLITH_EVENT_TOTAL];

      // Global Physics info
      Vector _gravityConst;
      float _dragConst;


    protected:
      Manager();


      // Load the input device configuration
      void _loadInput( Json::Value& );
      // Load the input device configuration
      void _loadAudio( Json::Value& );
      // Load the required fonts
      void _loadFonts( Json::Value& );
      // Configure the window
      void _loadWindow( Json::Value& );
      // Load all the texture files
      void _loadTextures( Json::Value& );
      // Load all the resources
      void _loadResources( Json::Value& );
      // Load the story
      void _loadStory( Json::Value& );

    public:
      virtual ~Manager();


      // Initialise the manager class from the configuration file
      void init( std::string );


      // Load the first scene into the engine and give it control
      void run();


      ////////////////////////////////////////////////////////////////////////////////
      // Get the pointers to various managers and builders

      // Return a pointer to the texture builder
      ObjectBuilder* getObjectFactory() { return _objectFactory; }

      // Return a pointer to the Scene builder
      SceneBuilder* getContextFactory() { return _contextFactory; }

      // Get the pointer to the window
      SDL_Renderer* getRendererPointer() { return _theRenderer; }

      // Get the pointer to the window
      Window& getWindow() { return _theWindow; }

      // Return a pointer to the required font data
      TTF_Font* getFontPointer( std::string );

      // Return a pointer to the input manager
      InputManager& getInputManager() { return _theInput; }

      // Return a pointer to the input manager
      AudioManager& getAudioManager() { return _theAudio; }


      ////////////////////////////////////////////////////////////////////////////////
      // Context Memory

      // Return a pointer to the required scene
      Context* getContext( size_t );

      // Return the ID number for a given scene
      unsigned int getContextID( std::string name ) const { return _contexts.getID( name ); }

      // Return a pointer to the current active context - may become invalid after rendering!
      Context* getCurrentContext() { return _theEngine.currentContext(); }


      ////////////////////////////////////////////////////////////////////////////////
      // Access Fonts and Colours

      // Return the default font
      TTF_Font* getDefaultFont() { return _defaultFont; }

      // Return the default colour
      SDL_Color& getDefaultColour() { return _defaultColor; }


      ////////////////////////////////////////////////////////////////////////////////
      // Texture and resource access functionality

      // Return a raw texture container with the given name
      RawTexture findRawTexture( std::string ) const;

      // Return the number of teams
      size_t getNumberTeams() const { return _teamNames.size(); }

      // Return the team ID for a given name
      unsigned int getTeamID( std::string name ) { return _teamNames[name]; }

      // Return a pointer to a given resource
      Drawable* getResource( std::string name ) { return _resources.get( name ); }

      // Return a pointer to a given resource
      Drawable* getResource( unsigned int i ) { return _resources[i]; }

      // Spawn a new instance of a resource and return the memory to the caller
      Drawable* spawn( std::string name, Vector pos = Vector(0.0) ) { return _resources.get( name )->cloneAt( pos ); }

      // Spawn a new instance of a resource and return the memory to the caller
      Drawable* spawn( unsigned int i, Vector pos ) { return _resources[i]->cloneAt( pos ); }


      ////////////////////////////////////////////////////////////////////////////////
      // User Event functions

      // Configure the list of user events for game events
      void configureEvents();

      // Push a user event into the SDL event queue
      void raiseEvent( RegolithEvent );


      ////////////////////////////////////////////////////////////////////////////////
      // Context stact Interface.
      // Pushed context stack operations to the engine

      // Open a new context on top of the stack
      void openContext( Context* );

      // Open a new context in place of the current one
      void transferContext( Context* );

      // Closes and then pops the context on the top of the stack
      void closeContext();

      // Resets the context stack to the provided context
      void setContextStack( Context* );


      ////////////////////////////////////////////////////////////////////////////////
      // Global Physics functions

      // Return the constant g
      inline Vector getGravity() { return _gravityConst; }

      // Return the drag
      inline float getDrag() { return _dragConst; }
  };

}

#endif // REGOLITH_MANAGER_H_


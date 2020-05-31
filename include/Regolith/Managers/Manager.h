
#ifndef REGOLITH_MANAGER_MANAGER_H_
#define REGOLITH_MANAGER_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/Architecture/FactoryTemplate.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Managers/HardwareManager.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Components/Window.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/GamePlay/Signal.h"
#include "Regolith/Utilities/Singleton.h"

#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Regolith
{
  // Forward declarations
  class Context;

  // Container typedefs
  typedef std::map< std::string, TTF_Font* > FontMap;
  typedef std::map< std::string, TeamID > TeamNameMap;

  // Factory typedefs
  typedef FactoryTemplate< GameObject, DataHandler& > ObjectFactory;
  typedef FactoryTemplate< Context > ContextFactory;
  typedef FactoryTemplate< Signal > SignalFactory;

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
      HardwareManager _theHardware;
      DataManager _theData;
      Engine _theEngine;
      SDL_Renderer* _theRenderer;
      IDNumber _entryPoint;

      // Factories to provide object/context creation
      ObjectFactory _objectFactory;
      ContextFactory _contextFactory;
      SignalFactory _signalFactory;

      // Owned memory for objects, contexts, etc
      FontMap _fonts;
      TeamNameMap _teamNames;

      // Containers of all the data loaded/to be loaded into memory
      NamedVector< Context, true > _contexts;

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
      // Load all the collision teams
      void _loadTeams( Json::Value& );
      // Load all the data manager
      void _loadData( Json::Value& );
      // Load all the resources
      void _loadGlobalGameObjects( Json::Value& );
      // Load all the resources
      void _loadContexts( Json::Value& );

    public:
      virtual ~Manager();


      // Initialise the manager class from the configuration file
      void init( std::string );


      // Load the first scene into the engine and give it control
      void run();


      ////////////////////////////////////////////////////////////////////////////////
      // Get the pointers to various managers and builders

      // Return a reference to the texture builder
      ObjectFactory& getObjectFactory() { return _objectFactory; }

      // Return a reference to the Scene builder
      ContextFactory& getContextFactory() { return _contextFactory; }

      // Return a reference to the Scene builder
      SignalFactory& getSignalFactory() { return _signalFactory; }

      // Get the pointer to the window
      SDL_Renderer* getRendererPointer() { return _theRenderer; }

      // Get the pointer to the window
      Window& getWindow() { return _theWindow; }

      // Return a pointer to the required font data
      TTF_Font* getFontPointer( std::string );

      // Return a reference to the input manager
      InputManager& getInputManager() { return _theInput; }

      // Return a reference to the input manager
      AudioManager& getAudioManager() { return _theAudio; }

      // Return a reference to the data manager
      DataManager& getDataManager() { return _theData; }

      // Return a reference to the global data handler manager
      const DataHandler& getGlobalData() { return _theData.globalData(); }


      ////////////////////////////////////////////////////////////////////////////////
      // Context Memory

      // Asks the manager to build a context and store the memory. Returns a pointer to the new context
      Context* buildContext( Json::Value& );

      // Return a pointer to a requested context - creating an entry if one doesn't exist
      IDNumber requestContext( std::string name ) { return _contexts.addName( name ); }

      // Return the name of a given context. Mostly useful for debugging!
      std::string getContextName( IDNumber id ) { return _contexts.getName( id ); }

      // Return a pointer to a requested context
      Context* getContext( IDNumber id ) { return _contexts[ id ]; }

      // Return a pointer to the current active context - may become invalid after rendering!
      Context* getCurrentContext() { return _theEngine.currentContext(); }


      ////////////////////////////////////////////////////////////////////////////////
      // Access Fonts and Colours

      // Return the default font
      TTF_Font* getDefaultFont() { return _defaultFont; }

      // Return the default colour
      SDL_Color& getDefaultColour() { return _defaultColor; }


      ////////////////////////////////////////////////////////////////////////////////
      // Team Configuration

      // Return the number of teams
      size_t getNumberTeams() const { return _teamNames.size(); }

      // Return the team ID for a given name
      TeamID getTeamID( std::string name );

      // Add a team to the map
      void addTeam( std::string name, IDNumber id ) { _teamNames[name] = id; }


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
      void openContext( IDNumber );

      // Open a new context in place of the current one
      void transferContext( IDNumber );

      // Closes and then pops the context on the top of the stack
      void closeContext();

      // Resets the context stack to the provided context
      void setContextStack( IDNumber );


      ////////////////////////////////////////////////////////////////////////////////
      // Global Physics functions

      // Return the constant g
      inline Vector getGravity() { return _gravityConst; }

      // Return the drag
      inline float getDrag() { return _dragConst; }
  };

}

#endif // REGOLITH_MANAGER_H_


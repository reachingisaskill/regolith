
#ifndef REGOLITH_MANAGER_MANAGER_H_
#define REGOLITH_MANAGER_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/Link.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/FactoryTemplate.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Managers/HardwareManager.h"
#include "Regolith/Managers/CollisionManager.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Managers/FontManager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Components/Window.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/GamePlay/Signal.h"
#include "Regolith/Contexts/Context.h"
#include "Regolith/Utilities/Singleton.h"

#include <vector>
#include <map>
#include <string>
#include <deque>


namespace Regolith
{

  // Factory typedefs
  typedef FactoryTemplate< GameObject, ContextGroup& > ObjectFactory;
  typedef FactoryTemplate< Context, ContextGroup& > ContextFactory;

  // Manager class
  // Global storage for all scenes, renderers and windows.
  // Current version limited to a single window.
  class Manager : public Singleton< Manager >
  {
    friend class Singleton< Manager >; // Required to enable the singleton pattern

    template < class T, class R > friend class Link;

    private:
      // Crucial objects for Regolith
      ThreadManager _theThreads;
      Window _theWindow;
      bool _rendererExists;
      InputManager _theInput;
      AudioManager _theAudio;
      HardwareManager _theHardware;
      CollisionManager _theCollision;
      DataManager _theData;
      ContextManager _theContexts;
      FontManager _theFonts;
      Engine _theEngine;

      // Factories to provide object/context creation
      ObjectFactory _objectFactory;
      ContextFactory _contextFactory;

      // Maps of the collision team and type names
      TeamNameMap _teamNames;
      TypeNameMap _typeNames;

      // Useful global constants
      std::string _title;

      // Info for SDL user events
      Uint32 _eventStartIndex;
      SDL_Event _gameEvents[REGOLITH_EVENT_TOTAL];


    protected:
      Manager();

      // Load the input device configuration
      void _loadInput( Json::Value& );
      // Load the input device configuration
      void _loadAudio( Json::Value& );
      // Configure the window
      void _loadWindow( Json::Value& );
      // Load map of collision teams
      void _loadCollision( Json::Value& );
      // Load all the data manager
      void _loadData( Json::Value& );
      // Load all the requested fonts
      void _loadFonts( Json::Value& );
      // Load all the resources
      void _loadGlobalGameObjects( Json::Value& );
      // Load all the contexts
      void _loadContexts( Json::Value& );

      // Configure the list of user events for game events
      void _configureEvents();


    public:
      virtual ~Manager();


      // Initialise the manager class from the configuration file
      void init( std::string );


      // Load the first context group into the engine and give it control
      bool run();


      // Sends quit an error flags to all threads and rejoins them if possible
      void error() { _theThreads.error(); }


      // Create and return the pointer to the Renderer - can only be called once!
      Camera& requestCamera();


      ////////////////////////////////////////////////////////////////////////////////
      // Get the pointers to various managers and builders

      // Return a reference to the texture builder
      ObjectFactory& getObjectFactory() { return _objectFactory; }

      // Return a reference to the Scene builder
      ContextFactory& getContextFactory() { return _contextFactory; }



      // Return a reference to the thread manager
      ThreadManager& getThreadManager() { return _theThreads; }

      // Return a reference to the input manager
      InputManager& getInputManager() { return _theInput; }

      // Return a reference to the input manager
      AudioManager& getAudioManager() { return _theAudio; }

      // Return a reference to the data manager
      DataManager& getDataManager() { return _theData; }

      // Return a reference to the font manager
      FontManager& getFontManager() { return _theFonts; }


      ////////////////////////////////////////////////////////////////////////////////
      // User Event functions

      // Push a user event into the SDL event queue
      void raiseEvent( RegolithEvent );


      ////////////////////////////////////////////////////////////////////////////////
      // Link access functions
      // If the requested link is not allowed a compile time error will be shown.

      template < class REQUESTER >
      Link< Manager, REQUESTER > getManager() { return Link< Manager, REQUESTER >( *this ); }

      template < class REQUESTER >
      Link< Engine, REQUESTER > getEngine() { return Link< Engine, REQUESTER >( _theEngine ); }

      template < class REQUESTER >
      Link< Engine, REQUESTER > getWindow() { return Link< Engine, REQUESTER >( _theWindow ); }

      template < class REQUESTER >
      Link< Window, REQUESTER > getWindow() { return Link< Window, REQUESTER >( _theWindow ); }

      template < class REQUESTER >
      Link< CollisionManager, REQUESTER > getCollisionManager() { return Link< CollisionManager, REQUESTER >( _theCollision ); }

      template < class REQUESTER >
      Link< ContextManager, REQUESTER > getContextManager() { return Link< ContextManager, REQUESTER >( _theContexts ); }

  };

}

#endif // REGOLITH_MANAGER_H_


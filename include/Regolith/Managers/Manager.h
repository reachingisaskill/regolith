
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
      ~Manager();


      // Initialise the manager class from the configuration file
      void init( std::string );


      // Load the first context group into the engine and give it control
      bool run();


      ////////////////////////////////////////////////////////////////////////////////
      // Functions accessible to contexts and objects during run time

      // Events during game play

      // Sends an error flag to all threads and triggers all registered condition variables
      void error();

      // Push a game/regolith event into the SDL event queue
      void raiseEvent( RegolithEvent );


      // Collision Interface

      // Return a collision team id
      CollisionTeam getCollisionTeam( std::string );

      // Return a collision type id
      CollisionType getCollisionType( std::string );


      // Music Interface

      // Add a track to the back of the queue
      void queueTrack( Music* );

      // Clear the current queue and play this track instead
      void playTrack( Music* );

      // Clear the queue
      void clearQueue();

      // Stop the the audio if its playing
      void stopTrack();

      // Pause the the audio if its playing
      void pauseTrack();

      // Resume the paused audio
      void resumeTrack();

      // Stop the infinitely repeating track
      void stopRepeatTrack();

      // Stop the current track and move on to the next one in the queue
      void nextTrack();

      // Skip to the next track at the end of the current repeat
      void nextRepeatTrack();


      ////////////////////////////////////////////////////////////////////////////////
      // Link access functions
      // If the requested link is not allowed a compile time error will be shown.

//      template < class REQUESTER >
//      Link< Manager, REQUESTER > getManager() { return Link< Manager, REQUESTER >( *this ); }

      template < class REQUESTER >
      Link< ThreadManager, REQUESTER > getThreadManager() { return Link< ThreadManager, REQUESTER >( _theThreads ); }

      template < class REQUESTER >
      Link< Window, REQUESTER > getWindow() { return Link< Window, REQUESTER >( _theWindow ); }

      template < class REQUESTER >
      Link< InputManager, REQUESTER > getInputManager() { return Link< InputManager, REQUESTER >( _theInput ); }


      template < class REQUESTER >
      Link< HardwareManager, REQUESTER > getHardwareManager() { return Link< HardwareManager, REQUESTER >( _theHardware ); }

      template < class REQUESTER >
      Link< CollisionManager, REQUESTER > getCollisionManager() { return Link< CollisionManager, REQUESTER >( _theCollision ); }


      template < class REQUESTER >
      Link< ContextManager, REQUESTER > getContextManager() { return Link< ContextManager, REQUESTER >( _theContexts ); }

      template < class REQUESTER >
      Link< Engine, REQUESTER > getEngine() { return Link< Engine, REQUESTER >( _theEngine ); }


      ////////////////////////////////////////////////////////////////////////////////
      // Get the pointers to various managers and builders

      // Return a reference to the texture builder
      ObjectFactory& getObjectFactory() { return _objectFactory; }

      // Return a reference to the Scene builder
      ContextFactory& getContextFactory() { return _contextFactory; }


      // Return a reference to the input manager
//      AudioManager& getAudioManager() { return _theAudio; }

      // Return a reference to the data manager
      DataManager& getDataManager() { return _theData; }

      // Return a reference to the font manager
      FontManager& getFontManager() { return _theFonts; }

  };

}

#endif // REGOLITH_MANAGER_H_


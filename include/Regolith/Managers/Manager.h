
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
#include "Regolith/Managers/WindowManager.h"
#include "Regolith/Managers/EngineManager.h"
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
      WindowManager _theWindow;
      InputManager _theInput;
      AudioManager _theAudio;
      HardwareManager _theHardware;
      CollisionManager _theCollision;
      DataManager _theData;
      ContextManager _theContexts;
      FontManager _theFonts;
      EngineManager _theEngine;

      // Factories to provide object/context creation
      ObjectFactory _objectFactory;
      ContextFactory _contextFactory;

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

      // Play the provided playlist
      void loadPlaylist( Playlist* );

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


      // Window interface

      // Set the window title
      void setWindowTitle( std::string );


      // Fonts interface

      // Return an Pen to write text
      Pen requestPen( std::string, unsigned int, SDL_Color );


      ////////////////////////////////////////////////////////////////////////////////
      // Get the pointers to various managers and builders

      // Return a reference to the texture builder
      ObjectFactory& getObjectFactory() { return _objectFactory; }

      // Return a reference to the Scene builder
      ContextFactory& getContextFactory() { return _contextFactory; }


      ////////////////////////////////////////////////////////////////////////////////
      // Link access functions
      // If the requested link is not allowed a compile time error will be shown.

      template < class REQUESTER >
      Link< ThreadManager, REQUESTER > getThreadManager() { return Link< ThreadManager, REQUESTER >( _theThreads ); }

      template < class REQUESTER >
      Link< WindowManager, REQUESTER > getWindowManager() { return Link< WindowManager, REQUESTER >( _theWindow ); }

      template < class REQUESTER >
      Link< InputManager, REQUESTER > getInputManager() { return Link< InputManager, REQUESTER >( _theInput ); }

      template < class REQUESTER >
      Link< AudioManager, REQUESTER > getAudioManager() { return Link< AudioManager, REQUESTER >( _theAudio ); }

      template < class REQUESTER >
      Link< HardwareManager, REQUESTER > getHardwareManager() { return Link< HardwareManager, REQUESTER >( _theHardware ); }

      template < class REQUESTER >
      Link< CollisionManager, REQUESTER > getCollisionManager() { return Link< CollisionManager, REQUESTER >( _theCollision ); }

      template < class REQUESTER >
      Link< DataManager, REQUESTER > getDataManager() { return Link< DataManager, REQUESTER >( _theData ); }

      template < class REQUESTER >
      Link< ContextManager, REQUESTER > getContextManager() { return Link< ContextManager, REQUESTER >( _theContexts ); }

      template < class REQUESTER >
      Link< FontManager, REQUESTER > getFontManager() { return Link< FontManager, REQUESTER >( _theFonts ); }

      template < class REQUESTER >
      Link< EngineManager, REQUESTER > getEngineManager() { return Link< EngineManager, REQUESTER >( _theEngine ); }

  };

}

#endif // REGOLITH_MANAGER_H_



#ifndef REGOLITH_CONTEXTS_PLATFORMER_H_
#define REGOLITH_CONTEXTS_PLATFORMER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/GameObjects/ControllableCharacter.h"
#include "Regolith/Utilities/NamedReferenceVector.h"


namespace Regolith
{

  class Platformer : public Context
  {
    private:
      // Music to play at the start of the scene
      unsigned int _defaultMusic;

      // Pointer to the pause menu context
      unsigned int _pauseMenu;

      // Memory is owned by the Manager
      unsigned int _playerID;
      ControllableCharacter* _player;

      // Data for player spawning
      NamedReferenceVector< Vector > _spawnPoints;
      unsigned int _currentPlayerSpawn;

    protected:
      // Hooks for context state changes
      virtual void onStart() override;
      virtual void onStop() override;
      virtual void onPause() override;
      virtual void onResume() override;

    public:
      // Constructor
      Platformer();

      // Destructor
      virtual ~Platformer();

      // Configure the Scene
      virtual void configure( Json::Value& ) override;

      // Validate the Scene
      virtual void validate() const override;


      // Returns true to pause and ignore the parent context. This prevents it from being rendered, updated, stepped, anything!
      // Use this for when child contexts/menus/etc required the full screen and comply obscure the parent
      virtual bool overridesPreviousContext() const override { return true; }


      // Respawn the player at the last spawn point
      void playerRespawn();


//////////////////////////////////////////////////
      // Requirements for the ControllableInterface - input action handling

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) override;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) override;
      virtual void booleanAction( const InputAction&, bool ) override;
      virtual void floatAction( const InputAction&, float ) override;
      virtual void vectorAction( const InputAction&, const Vector& ) override;
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override;

      
//////////////////////////////////////////////////
      // Requirements for a component - event handling

      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
  };

}


#endif // REGOLITH_CONTEXTS_PLATFORMER_H_


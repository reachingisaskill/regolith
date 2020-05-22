
#ifndef REGOLITH_GAMEOBJECTS_CONTROLLABLE_CHARACTER_H_
#define REGOLITH_GAMEOBJECTS_CONTROLLABLE_CHARACTER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Controllable.h"
#include "Regolith/GameObjects/CharacterBase.h"


namespace Regolith
{

  class ControllableCharacter : public Controllable, public CharacterBase
  {
    private:

    public :
      // Con/De-structors
      ControllableCharacter() : Controllable(), CharacterBase() {}

      virtual ~ControllableCharacter() {}

      // Configuration
      virtual void configure( Json::Value& ) override;

      // Replace the character at the specified position
      virtual void respawn( const Vector& vec ) override { setPosition( vec ); setVelocity( Vector( 0.0, 0.0 ) ); }


////////////////////////////////////////////////////////////////////////////////
      // Controllable interface
      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) = 0;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) = 0;
      virtual void booleanAction( const InputAction&, bool ) = 0;
      virtual void floatAction( const InputAction&, float ) = 0;
      virtual void vectorAction( const InputAction&, const Vector& ) = 0;
      virtual void mouseAction( const InputAction&, bool, const Vector& ) = 0;
  };

}

#endif // REGOLITH_GAMEOBJECTS_CONTROLLABLE_CHARACTER_H_


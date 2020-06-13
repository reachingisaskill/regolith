
#ifndef REGOLITH_TEST_TEST_CHARACTER_H_
#define REGOLITH_TEST_TEST_CHARACTER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GameObjects/ControllableCharacter.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Timer.h"


namespace Regolith
{
  class DataHandler;

  class TestCharacter : public ControllableCharacter
  {
    private :
      Texture _theTexture;
      SDL_Rect _destination;
      float _movementForce;
      float _jumpSpeed;
      RawSound* _jumpSound;
      RawSound* _hardLandingSound;

      Timer _fallTimer;

    protected:
      void setMovementForce( float f ) { _movementForce = f; }
      void setJumpSpeed( float f ) { _jumpSpeed = f; }
      void setJumpSound( RawSound* s ) { _jumpSound = s; }
      void setHardSound( RawSound* s ) { _hardLandingSound = s; }


      // Required for it to be noisy
      virtual void registerSoundEffects( AudioHandler* ) override;

    public :
      // Con/De-structors
      TestCharacter();
      virtual ~TestCharacter();


      // Configure the class
      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override;

      // Validate the configuration
      virtual void validate() const override;




      // Required to be drawable
      virtual void render( const Camera& ) override;


      // Required to be animated
      void update( float ) override;


      // Not clonable - but required to be physical
      virtual PhysicalObject* clone( const Vector& ) const override { return nullptr; }


      // Required to be collidable
      virtual void onCollision( const Vector&, float, const Collidable* ) override;


      // Override the width/height getters
      virtual float getWidth() const override { return _theTexture.getWidth(); }
      virtual float getHeight() const override { return _theTexture.getHeight(); }


////////////////////////////////////////////////////////////////////////////////
      // Controllable interface
      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) override;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) override {}
      virtual void booleanAction( const InputAction&, bool ) override;
      virtual void floatAction( const InputAction&, float ) override {}
      virtual void vectorAction( const InputAction&, const Vector& ) override {}
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override {}

  };

}

#endif // REGOLITH_TEST_TEST_CHARACTER_H_


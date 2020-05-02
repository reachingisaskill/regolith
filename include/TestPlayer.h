
#ifndef REGOLITH_TEST_PLAYER_H_
#define REGOLITH_TEST_PLAYER_H_

#include "Definitions.h"

#include "Texture.h"
#include "Sprite.h"
#include "ObjectFactory.h"
#include "Timer.h"

#include <json/json.h>


namespace Regolith
{

  class TestPlayer : public Sprite
  {
    private :
      float _movementForce;
      float _jumpSpeed;
      unsigned int _jumpSound;
      unsigned int _hardLandingSound;

      Timer _fallTimer;

    public :
      TestPlayer();

      explicit TestPlayer( Texture );

      virtual ~TestPlayer();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      //virtual bool hasCollision() const;
      virtual bool hasInput() const { return true; }
      //virtual bool hasAnimation() const;

      // Perform the steps to call SDL_RenderCopy, etc
      //virtual void render( Camera* ) = 0;

      // Update the objects behaviour based on the provided timestep
      //virtual void update( Uint32 t ) { this->step( t ); }


      // Returns the number of collision objects for the class.
      // The argument is a pointer which will be updated to point at the first object
      //virtual unsigned int getCollision( Collision*& ) = 0;

      // Create a new copy and return the pointer to it. Transfers ownership of the memory
      virtual Drawable* clone() const;

      // Update the objects behaviour based on the provided timestep
      virtual void registerActions( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}

      void setMovementForce( float f ) { _movementForce = f; }

      void setJumpSpeed( float f ) { _jumpSpeed = f; }

      void setJumpSound( unsigned int i ) { _jumpSound = i; }
      void setHardSound( unsigned int i ) { _hardLandingSound = i; }


      void onCollision( unsigned int, const Vector& );
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

  class TestPlayerFactory : public ObjectFactory_base 
  {
    private:

    public:
      TestPlayerFactory();

      virtual ~TestPlayerFactory();

      virtual const char* getObjectName() const { return "test_player"; }

      virtual Drawable* build( Json::Value& ) const;
  };

}

#endif // REGOLITH_TEST_PLAYER_H_


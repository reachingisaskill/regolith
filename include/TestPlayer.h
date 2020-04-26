
#ifndef __REGOLITH__TEST_PLAYER_H__
#define __REGOLITH__TEST_PLAYER_H__

#include "Definitions.h"

#include "Sprite.h"
#include "SpriteSheet.h"
#include "ObjectFactory.h"

#include <json/json.h>


namespace Regolith
{

  class TestPlayer : public Sprite
  {
    private :
      float _movementForce;
      float _jumpSpeed;

    public :
      TestPlayer();

      explicit TestPlayer( SpriteSheet );

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
      virtual void registerEvents( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}

      void setMovementForce( float f ) { _movementForce = f; }

      void setJumpSpeed( float f ) { _jumpSpeed = f; }
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

#endif // __REGOLITH__TEST_PLAYER_H__


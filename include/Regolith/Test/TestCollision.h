
#ifndef REGOLITH_TEST_TEST_COLLISION_H_
#define REGOLITH_TEST_TEST_COLLISION_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Collidable.h"


namespace Regolith
{
  class DataHandler;

  class TestCollision : public Collidable
  {
    private :
      bool _collided;
      Vector _lastNormal;
      float _lastOverlap;

    public :
      // Con/De-structors
      TestCollision();
      virtual ~TestCollision();


      // Configure the class
      virtual void configure( Json::Value&, DataHandler& ) override;

      // Validate the configuration
      virtual void validate() const override;


      // Not clonable - but required to be physical
      virtual PhysicalObject* clone( const Vector& ) const override { return nullptr; }


      // Required to be collidable
      virtual void onCollision( const Vector&, float, const Collidable* ) override;

      bool collided() const { return _collided; }
      Vector lastNormal() const { return _lastNormal; }
      float lastOverlap() const { return _lastOverlap; }
      void reset() { _collided = false; }
  };

}

#endif // REGOLITH_TEST_TEST_COLLISION_H_


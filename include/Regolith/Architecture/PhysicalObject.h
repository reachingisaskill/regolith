
#ifndef REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_
#define REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"

namespace Regolith
{

  /*
   * Defines the interface for "physical objects" any object that can be cloned, moved, seen or collided with.
   * Must therefore have a position and an area.
   */
  class PhysicalObject : virtual public GameObject
  {
    private:
      bool _destroyMe;
      Vector _position;
      float _mass;
      float _inverseMass;
      float _rotation;

    protected :
      void setMass( float );

    public:
      PhysicalObject();

      virtual ~PhysicalObject() {}


      void configure( Json::Value&, DataHandler& ) override;


      // Tells the user that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Function to create to an copied instance at the specified position
      virtual PhysicalObject* clone( const Vector& ) const = 0;


      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }


      const float& getMass() const { return _mass; }
      const float& getInverseMass() const { return _inverseMass; }

      const Vector& position() const { return _position; }
      Vector getPosition() const { return _position; }
      void setPosition( Vector p ) { _position = p; }

      const float& rotation() const { return _rotation; }
      float getRotation() const { return _rotation; }
      void setRotation( float r ) { _rotation = r; }

      void move( Vector m ) { _position += m; }
      void rotate( float r ) { _rotation += r; }

      virtual float getWidth() const = 0;
      virtual float getHeight() const = 0;

  };

}

#endif // REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_


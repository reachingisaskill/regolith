
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
      float _width;
      float _height;
      float _rotation;

    protected :

    public:
      PhysicalObject();

      PhysicalObject( Vector, float, float, float r = 0 );

      virtual ~PhysicalObject() {}



      // Tells the user that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Function to create to an copied instance at the specified position
      virtual PhysicalObject* clone( const Vector& ) const = 0;


      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }


      const Vector& position() { return _position; }
      Vector getPosition() const { return _position; }
      void move( Vector m ) { _position += m; }
      void rotate( float r ) { _rotation += r; }

      float getWidth() const { return _width; }
      float getHeight() const { return _height; }
      float getRotation() const { return _rotation; }

      void setPosition( Vector p ) { _position = p; }
      void setWidth( float w ) { _width = w; }
      void setHeight( float h ) { _height = h; }
      void setRotation( float r ) { _rotation = r; }
  };

}

#endif // REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_


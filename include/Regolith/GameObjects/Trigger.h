
#ifndef REGOLITH_GAMEOBJECTS_TRIGGER_H_
#define REGOLITH_GAMEOBJECTS_TRIGGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Interactable.h"


namespace Regolith
{
  class DataHandler;

  class Trigger : public Collidable, public Interactable
  {
    private:
      float _width;
      float _height;

    protected:

    public:
      Trigger();

      virtual ~Trigger();


      // Configure the object
      virtual void configure( Json::Value&, DataHandler& ) override;


      // Override the width/height getters
      virtual float getWidth() const override { return _width; }
      virtual float getHeight() const override { return _height; }


      // Function called when a collision is found with another object
      // Overlap vector, other object pointer
      virtual void onCollision( const Vector&, float, const Collidable* ) override { this->trigger(); }


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;
  };

}

#endif // REGOLITH_GAMEOBJECTS_TRIGGER_H_


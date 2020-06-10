
#ifndef REGOLITH_GAMEOBJECTS_REGION_H_
#define REGOLITH_GAMEOBJECTS_REGION_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Collidable.h"


namespace Regolith
{
  class DataHandler;

  class Region : public Collidable
  {
    private:
      float _width;
      float _height;

    protected:

    public:
      Region();

      virtual ~Region();


      // Configure the object
      virtual void configure( Json::Value&, DataHandler& ) override;


      // Override the width/height getters
      virtual float getWidth() const override { return _width; }
      virtual float getHeight() const override { return _height; }


      // Function called when a collision is found with another object
      // Overlap vector, other object pointer
      virtual void onCollision( const Vector&, float, const Collidable* ) override {}


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;
  };

}

#endif // REGOLITH_GAMEOBJECTS_REGION_H_


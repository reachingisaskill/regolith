
#ifndef REGOLITH_COLLISIONS_TILESHEET_COLLISION_H_
#define REGOLITH_COLLISIONS_TILESHEET_COLLISION_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Collisions/Collision.h"

#include <vector>


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Class for storing collisionable hitboxes for tiled objects

  class TilesheetCollision : public Collision
  {

    private:
      // Store a vector of vector of hit boxes. Only one set is active at time.
      Collision::HitBoxVector _hitboxes;

    public:
      TilesheetCollision();
      virtual ~TilesheetCollision() {}


      // Configure the hitboxes for each frame
      virtual void configure( Json::Value& ) override;


      // Return the number of hitboxes in the current frame
      virtual size_t size() const override { return _hitboxes.size(); }

      // Return the start of the active collision vector
      virtual iterator begin() const override { return _hitboxes.begin(); }

      // Return the end of the active collision vector
      virtual iterator end() const override { return _hitboxes.end(); }

  };

}

#endif // REGOLITH_COLLISIONS_TILESHEET_COLLISION_H_


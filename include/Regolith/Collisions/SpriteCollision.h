
#ifndef REGOLITH_COLLISIONS_SPRITE_COLLISION_H_
#define REGOLITH_COLLISIONS_SPRITE_COLLISION_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Collisions/Collision.h"

#include <vector>


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Class for storing collisionable hitboxes for sprite-based objects

  class SpriteCollision : public Collision
  {

    private:
      // Store a vector of vector of hit boxes. Only one set is active at time.
      Collision::Frames _collisionFrames;

      // Current active hitboxes
      size_t _currentCollision;

    public:
      SpriteCollision();
//      ~SpriteCollision();

      // Configure the hitboxes for each frame
      void configure( Json::Value& );


      // Return the total number of frames loaded
      unsigned int getNumberFrames() const { return _collisionFrames.size(); }

      // If this is an animated collision, update accordingly
      void setFrameNumber( unsigned int frame ) { _currentCollision = frame; }


      // Return the number of hitboxes in the current frame
      virtual size_t size() const override { return _collisionFrames[_currentCollision].size(); }

      // Return the start of the active collision vector
      virtual iterator begin() const override { return _collisionFrames[_currentCollision].begin(); }

      // Return the end of the active collision vector
      virtual iterator end() const override { return _collisionFrames[_currentCollision].end(); }

  };

}

#endif // REGOLITH_COLLISIONS_SPRITE_COLLISION_H_


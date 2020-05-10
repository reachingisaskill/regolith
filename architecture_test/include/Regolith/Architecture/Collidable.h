
#ifndef REGOLITH_ARCHITECTURE_COLLIDABLE_H_
#define REGOLITH_ARCHITECTURE_COLLIDABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/GamePlay/Collision.h"


namespace Regolith
{
  namespace Architecture
  {

    /*
     * Defines the interface for object that can be collide. 
     * Main collision logic is within the Collision class definition.
     */
    class Collidable : virtual public PhysicalObject
    {
      private:
        Collision _collision;
        bool _collisionActive;
        unsigned int _collisionType;
        int _teamID;

      protected:
        void setCollision( Collision c ) { _collision = c; }
        void setCollisionActive( bool c ) { _collisionActive = c; }
        void setCollisionType( unsigned int t ) { _collisionType = t; }
        void setTeam( int t ) { _teamID = t; }

      public:
        Collidable();

        virtual ~Collidable() {}


        // Return the team id - determines collision interactions
        int getTeam() const { return _teamID; }


        // Signifies that the object has the collidable interface
        virtual bool hasCollision() const override { return true; }


        // Shortcut function. Skip collision if its not active
        bool collisionActive() const { return _collisionActive; }


        // Returns the type of collision interaction that occurs with this object
        unsigned int collisionType() const { return _collisionType; }


        // Function called when a collision is found with another object
        void onCollision( unsigned int, const Vector& ) = 0;

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_COLLIDABLE_H_


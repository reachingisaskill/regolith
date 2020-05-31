
#include "Regolith/Test/TestCollision.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  TestCollision::TestCollision() :
    Collidable(),
    _collided( false ),
    _lastNormal( 0.0 ),
    _lastOverlap( 0.0 )
  {
    Collision c;
    c.configure( Vector( 0.0, 1.0 ), 2.0, 3.0 );
    setCollision( c );
    setCollisionActive( true );
    setCollisionType( 5 );
    setTeam( 1 );
    setMass( 1.0 );
  }


  TestCollision::~TestCollision()
  {
  }


  void TestCollision::onCollision( const Vector& vec, float ov, const Collidable* )
  {
    _collided = true;
    _lastNormal = vec;
    _lastOverlap = ov;
  }


  void TestCollision::configure( Json::Value&, DataHandler& )
  {
//    Collidable::configure( json_data, handler );
  }

  void TestCollision::validate() const
  {
  }

}


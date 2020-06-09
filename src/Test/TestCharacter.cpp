
#include "Regolith/Test/TestCharacter.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  TestCharacter::TestCharacter() :
    ControllableCharacter(),
    _theTexture(),
    _destination( { 0, 0, 0, 0 } ),
    _movementForce( 0.005 ),
    _jumpSpeed( 3 ),
    _jumpSound( nullptr ),
    _hardLandingSound( nullptr ),
    _fallTimer()
  {
  }


  TestCharacter::~TestCharacter()
  {
  }


  void TestCharacter::registerSoundEffects( AudioHandler* handler )
  {
    handler->requestChannel( _jumpSound );
    handler->requestChannel( _hardLandingSound );
  }


  void TestCharacter::registerActions( InputHandler& handler )
  {
    DEBUG_LOG( "Registering player input requests" );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_UP );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_DOWN );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_LEFT );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_RIGHT );
    handler.registerInputRequest( this, INPUT_ACTION_JUMP );
    handler.registerInputRequest( this, INPUT_ACTION_ATTACK );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_HORIZONTAL );
    handler.registerInputRequest( this, INPUT_ACTION_MOVE_VERTICAL );
  }


  void TestCharacter::booleanAction( const InputAction& action, bool value )
  {
    static Manager* man = Manager::getInstance();

    float speed = _movementForce / man->getDrag();
    DEBUG_STREAM << " TestCharacter : " << (int)action << " : " << value << ", " << speed;

    switch ( action )
    {
      case INPUT_ACTION_MOVE_UP :
        break;

      case INPUT_ACTION_MOVE_DOWN :
        break;

      case INPUT_ACTION_MOVE_LEFT :
        if ( value )
        {
          this->velocity().x() -= speed;
          this->inputForce().x() -= _movementForce;
        }
        else
        {
          this->velocity().x() += speed;
          this->inputForce().x() += _movementForce;
        }
        break;

      case INPUT_ACTION_MOVE_RIGHT :
        if ( value )
        {
          this->velocity().x() += speed;
          this->inputForce().x() += _movementForce;
        }
        else
        {
          this->velocity().x() -= speed;
          this->inputForce().x() -= _movementForce;
        }
        break;

      case INPUT_ACTION_JUMP :
        if ( value && ( std::fabs( this->velocity().y() ) < epsilon ) )
        {
          this->velocity().y() -= _jumpSpeed;
         playSound( _jumpSound );
        }
        break;

      default :
        break;
    }

    DEBUG_STREAM << " TestCharacter: " << this->velocity();
  }


  void TestCharacter::render( const Camera& camera )
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );
    DEBUG_STREAM << "TestCharacter RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.x << ", " << destination.y << ", " << destination.w << ", " << destination.h;

    _theTexture.draw( &destination );
  }


  void TestCharacter::update( float time )
  {
    _theTexture.update( time );
  }


  void TestCharacter::onCollision( const Vector& normal, float overlap, const Collidable* object )
  {
    switch ( object->getTeam() )
    {
      case DEFAULT_TEAM_ENVIRONMENT :
        if ( normal.y() < 0 )
        {
          Uint32 falltime = _fallTimer.lap();
          if ( falltime > 500 || velocity().y() < -3.0 )
          {
            playSound( _hardLandingSound );
          }
        }
        break;

      default :
        break;
    }

    DEBUG_STREAM << getVelocity()%normal << normal;
    this->move( overlap*normal );
    this->addVelocity( getVelocity().fabs()%normal );

    DEBUG_STREAM << "On Collision Player :" << object->getTeam() << ", " << overlap << " , " << normal << " -- Pos " << getPosition() << ", Vel " << getVelocity();
  }


  void TestCharacter::configure( Json::Value& json_data, DataHandler& handler )
  {
    ControllableCharacter::configure( json_data, handler );

    _theTexture.configure( json_data, handler );

    // Set the current sprite position
    _destination.x = position().x();
    _destination.y = position().y();
    _destination.w = _theTexture.getWidth();
    _destination.h = _theTexture.getHeight();

    setWidth( _theTexture.getWidth() );
    setHeight( _theTexture.getHeight() );
  }

  void TestCharacter::validate() const
  {
  }

}


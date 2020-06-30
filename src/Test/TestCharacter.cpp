
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
          DEBUG_STREAM << "TestCharacter Playing Jump Sound : " << _jumpSound->sound;
          playSound( _jumpSound );
        }
        break;

      default :
        break;
    }
  }


  void TestCharacter::render( const Camera& camera )
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    _destination.w = getWidth();
    _destination.h = getHeight();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );

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
  }


  void TestCharacter::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    ControllableCharacter::configure( json_data, cg, handler );

    _theTexture.configure( json_data, handler );

    Utilities::validateJson( json_data, "jump_sound", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "landing_sound", Utilities::JSON_TYPE_STRING );

    _jumpSound = handler.getRawSound( json_data["jump_sound"].asString() );
    _hardLandingSound = handler.getRawSound( json_data["landing_sound"].asString() );

    // Set the current sprite position
    _destination.x = position().x();
    _destination.y = position().y();
  }

  void TestCharacter::validate() const
  {
  }

}


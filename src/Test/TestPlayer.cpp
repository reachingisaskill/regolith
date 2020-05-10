
#include "Test/TestPlayer.h"
#include "Managers/Manager.h"
#include "Managers/InputManager.h"
#include "Components/Utilities.h"

#include "logtastic.h"


namespace Regolith
{

  TestPlayer::TestPlayer() :
    Sprite(),
    _movementForce()
  {
  }


  TestPlayer::TestPlayer( Texture sheet ) :
    Sprite( sheet ),
    _movementForce()
  {
  }



  TestPlayer::~TestPlayer()
  {
  }


  Drawable* TestPlayer::clone() const
  {
    return (Drawable*) new TestPlayer( *this );
  }


  void TestPlayer::registerActions( InputHandler* handler )
  {
    DEBUG_LOG( "Registering player input requests" );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_UP );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_DOWN );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_LEFT );
    handler->registerInputRequest( this, INPUT_ACTION_MOVE_RIGHT );
    handler->registerInputRequest( this, INPUT_ACTION_JUMP );
  }


  void TestPlayer::booleanAction( const InputAction& action, bool value )
  {
    static Manager* man = Manager::getInstance();

    DEBUG_STREAM << " TestPlayer : " << (int)action << " : " << value;
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
          man->getAudioManager()->playChunk( _jumpSound );
        }
        break;

      default :
        break;
    }

    DEBUG_STREAM << " TestPlayer: " << this->velocity();
  }


  void TestPlayer::onCollision( unsigned int team, const Vector& overlap )
  {
    static Manager* man = Manager::getInstance();
    DEBUG_STREAM << "On Collision Player :" << team << ", " << overlap;

    switch ( team )
    {
      case DEFAULT_TEAM_ENVIRONMENT :
        if ( overlap.y() < 0 )
        {
          Uint32 falltime = _fallTimer.lap();
          if ( falltime > 500 || velocity().y() < -3.0 )
          {
            man->getAudioManager()->playChunk( _hardLandingSound );
          }
        }
        break;

      default :
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  TestPlayerFactory::TestPlayerFactory()
  {
  }


  TestPlayerFactory::~TestPlayerFactory()
  {
  }


  Drawable* TestPlayerFactory::build( Json::Value& json_data ) const
  {
    DEBUG_LOG( "Building Sprite" );
    Utilities::validateJson( json_data, "texture_name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "jump_speed", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "movement_force", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "jump_sound", Utilities::JSON_TYPE_STRING );

    std::string texture_name = json_data["texture_name"].asString();
    RawTexture texture = this->findRawTexture( texture_name );

    Texture sheet( texture );


    if ( json_data.isMember( "number_rows" ) && json_data.isMember( "number_columns" ) )
    {
      int num_rows = json_data["number_rows"].asInt();
      int num_cols = json_data["number_columns"].asInt();
      int num_used;
      if ( json_data.isMember( "number_used_cells" ) )
      {
        num_used = json_data["number_used_cells"].asInt();
      }
      else
      {
        num_used = 0;
      }
      sheet.configure( num_rows, num_cols, num_used );
    }
    else
    {
      // Only one sprite on the texture
      sheet.configure( 1, 1, 1 );
    }

    if ( json_data.isMember( "start_number" ) )
    {
      int start_num = json_data["start_number"].asInt();
      sheet.setSpriteNumber( start_num );
    }

    // Configure the default collision
    Collision* collision = nullptr;
    if ( json_data.isMember( "collision" ) )
    {
      if ( json_data["collision"].isString() && json_data["collision"].asString() == "default" )
      {
        DEBUG_LOG( "Configuring default collision for sprite" );
        collision = new Collision( Vector( 0.0, 0.0 ), sheet.getWidth(), sheet.getHeight() );
      }
      else if ( json_data["collision"].isObject() )
      {
        collision = this->buildCollision( json_data["collision"] );
      }
    }

    float jump_speed = json_data["jump_speed"].asFloat();
    float movement_force = json_data["movement_force"].asFloat();

    std::string jump_sound = json_data["jump_sound"].asString();
    std::string hard_sound = json_data["hard_landing_sound"].asString();

    TestPlayer* newPlayer = new TestPlayer( sheet );
    newPlayer->addCollision( collision );
    newPlayer->setJumpSpeed( jump_speed );
    newPlayer->setMovementForce( movement_force );
    newPlayer->setJumpSound( Manager::getInstance()->getAudioManager()->getEffectID( jump_sound ) );
    newPlayer->setHardSound( Manager::getInstance()->getAudioManager()->getEffectID( hard_sound ) );

    buildDrawable( newPlayer, json_data );

    return newPlayer;
  }
  

}

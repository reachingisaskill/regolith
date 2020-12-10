
#include "Regolith/StateObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // State details class

  StateObject::StateDetails::StateDetails() :
    _count( 0.0 ),
    _numberFrames( 0 ),
    id( 0 ),
    updatePeriod( 0.0 ),
    texture(),
    collision(),
    currentFrame( 0 )
  {
  }


  void StateObject::StateDetails::update( float timestep )
  {
    // Update the timer
    _count += timestep;

    // Determine how many frames have elapsed in the animation
    currentFrame += _count / updatePeriod;
    currentFrame = currentFrame % _numberFrames;

    // Reset the timer based on the update period
    _count = std::fmod( _count, updatePeriod );

    // Update the texture and collision objects
    texture.setFrameNumber( currentFrame );
    collision.setFrameNumber( currentFrame );

    DEBUG_STREAM << "StateDetails::update : _count : " << _count << ", frame No. : " << currentFrame << " of " << _numberFrames << ", update rate : " << updatePeriod;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Physical Object member functions


  StateObject::StateObject() :
    PhysicalObject(),
    _stateMap(),
    _startState(),
    _startStatePointer( nullptr ),
    _currentState( nullptr )
  {
  }


  StateObject::StateObject( const StateObject& other ) :
    PhysicalObject( other ),
    _stateMap( other._stateMap ),
    _startState( other._startState ),
    _startStatePointer( &_stateMap[_startState] ),
    _currentState( _startStatePointer )
  {
  }


  StateObject::~StateObject()
  {
  }


  void StateObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    PhysicalObject::configure( json_data, cg );

    Utilities::validateJson( json_data, "states", Utilities::JSON_TYPE_OBJECT );

    // Controlling the states
    Json::Value& states = json_data["states"];
    if ( states.size() == 0 )
    {
      Exception ex( "StateObject::configure()", "Every state object must have at least one state" );
      throw ex;
    }

    for( Json::Value::iterator s_it = states.begin(); s_it != states.end(); ++s_it )
    {
      std::string state_name = s_it.key().asString();
      Json::Value& state_data = (*s_it);

      _stateMap.emplace( std::make_pair( state_name, StateDetails() ) );

      // Texture details
      if ( Utilities::validateJson( state_data, "texture", Utilities::JSON_TYPE_OBJECT, this->hasTexture() ) )
      {
        _stateMap[ state_name ].texture.configure( state_data["texture"], cg.getDataHandler() );
      }
      
      // Hitbox details
      if ( Utilities::validateJson( state_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
      {
        _stateMap[ state_name ].collision.configure( state_data["collision"] );
      }

      // Animation details
      if ( Utilities::validateJson( state_data, "update_period", Utilities::JSON_TYPE_FLOAT, this->hasAnimation() ) )
      {
        _stateMap[ state_name ].updatePeriod = state_data[ "update_period" ].asFloat();
      }
    }

    // Store the reset state if given
    if ( Utilities::validateJson( json_data, "start_state", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string start_state_name = json_data["start_state"].asString();
      StateMap::iterator found = _stateMap.find( start_state_name );
      if ( found == _stateMap.end() )
      {
        Exception ex( "StateObject::configure()", "Requested start state not found in object configuration." );
        ex.addDetail( "Requested State Name", start_state_name );
        throw ex;
      }

      _startState = start_state_name;
      _startStatePointer = &found->second;
    }
    else
    {
      _startState = _stateMap.begin()->first;
      _startStatePointer = &_stateMap.begin()->second;
    }

    _currentState = _startStatePointer;

    DEBUG_STREAM << "StateObject::configure : Configured state object.";
  }


  void StateObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
    // Reset the current state and reset it's animations
    setState( _startStatePointer );
  }


  void StateObject::update( float timestep )
  {
    // Only need to update the current state
    _currentState->update( timestep );
  }

}


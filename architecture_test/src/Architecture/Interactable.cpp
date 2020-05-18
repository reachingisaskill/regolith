
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/GamePlay/Signal.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

namespace Regolith
{

  Interactable::Interactable() :
    _actions(),
    _triggerCount( 0 ),
    _triggerLimit( 0 ),
    _isLimited( false )
  {
  }


  Interactable::Interactable( const Interactable& in ) :
    _actions(),
    _triggerCount( in._triggerCount ),
    _triggerLimit( in._triggerLimit ),
    _isLimited( in._isLimited )
  {
    for ( SignalVector::const_iterator it = in._actions.begin(); it != in._actions.end(); ++it )
    {
      _actions.push_back( (*it)->clone() );
    }
  }


  void Interactable::trigger()
  {
    // See if we've hit the limit
    if ( _isLimited && ( _triggerCount >= _triggerLimit ) )
    {
      return;
    }

    // Trigger each signal.
    SignalVector::iterator end = _actions.end();
    for ( SignalVector::iterator it = _actions.begin(); it != end; ++it )
    {
      (*it)->trigger();
    }

    // Increment the counter
    ++_triggerCount;
  }


  void Interactable::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "signals", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["signals"], 0, Utilities::JSON_TYPE_OBJECT );

    SignalFactory& factory = Manager::getInstance()->getSignalFactory();
    Json::Value& signals = json_data["signals"];

    for ( Json::ArrayIndex i = 0; i < signals.size(); ++i )
    {
      addSignal( factory.build( signals[i] ) );
    }

  }


  void Interactable::validate() const
  {
    for ( SignalVector::const_iterator it = _actions.begin(); it != _actions.end(); ++it )
    {
      (*it)->validate();
    }
  }

}


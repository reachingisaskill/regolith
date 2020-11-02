
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/GamePlay/Signal.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

namespace Regolith
{

  Interactable::Interactable() :
    _copyCounter( 0 ),
    _actions(),
    _triggerCount( 0 ),
    _triggerLimit( 0 ),
    _isLimited( false )
  {
  }


  Interactable::Interactable( const Interactable& in ) :
    _copyCounter( in._copyCounter + 1 ),
    _actions( in._actions ),
    _triggerCount( in._triggerCount ),
    _triggerLimit( in._triggerLimit ),
    _isLimited( in._isLimited )
  {
  }


  Interactable& Interactable::operator=( const Interactable& in )
  {
    _copyCounter = in._copyCounter + 1;
    _actions = in._actions;
    _triggerCount = in._triggerCount;
    _triggerLimit = in._triggerLimit;
    _isLimited = in._isLimited;

    return *this;
  }


  Interactable::~Interactable()
  {
    // Only the original version can delete the actions
    // Spawned copies only own copies of the pointers
    if ( _copyCounter == 0 )
    {
      for ( SignalVector::iterator it = _actions.begin(); it != _actions.end(); ++it )
      {
        delete (*it);
      }
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


  void Interactable::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& hand )
  {
    Utilities::validateJson( json_data, "signals", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( json_data["signals"], 0, Utilities::JSON_TYPE_OBJECT );

    SignalFactory& factory = Manager::getInstance()->getSignalFactory();
    Json::Value& signals = json_data["signals"];

    for ( Json::ArrayIndex i = 0; i < signals.size(); ++i )
    {
      Signal* new_signal = factory.build( signals[i], cg, hand );
      addSignal( new_signal );
    }

  }

}


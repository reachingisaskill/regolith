
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/GamePlay/Signal.h"

namespace Regolith
{

  Interactable::Interactable() :
    _actions(),
    _triggerCount( 0 ),
    _triggerLimit( 0 ),
    _isLimited( false )
  {
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

}


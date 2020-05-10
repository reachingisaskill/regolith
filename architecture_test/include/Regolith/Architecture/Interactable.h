
#ifndef REGOLITH_ARCHITECTURE_INTERACTABLE_H_
#define REGOLITH_ARCHITECTURE_INTERACTABLE_H_

#include "Regolith/Global/Global.h"

#include <vector>


namespace Regolith
{
  class Signal;

  namespace Architecture
  {

    /*
     * Defines the interactable interface. Stores signal objects and allows them to be triggered by derived classes
     */
    class Interactable : public virtual GameObject
    {
      typedef std::vector< Signal* > SignalVector;

      private :
        SignalVector _actions;
        unsigned int _triggerCount;
        unsigned int _triggerLimit;
        bool _isLimited;

      protected:
        void setTriggerLimit( unsigned int l ) { _triggerLimit = l; }
        void setTriggerIsLimited( bool l ) { _isLimited = l; }

      public :
        Interactable();

        virtual ~Interactable() {}

        // Signify that this class is Interactable
        virtual bool hasInteraction() const override { return true; }


        // Return configuration
        unsigned int getTriggerCount() const { return _triggerCount; }
        unsigned int getTriggerLimit() const { return _triggerLimit; }
        unsigned int getTriggerIsLimited() const { return _isLimited; }


        // Add an action to the object
        void addAction( Signal* s ) { _actions.push_back( s ); }
        

        // Activate all the signals
        void trigger();
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_INTERACTABLE_H_


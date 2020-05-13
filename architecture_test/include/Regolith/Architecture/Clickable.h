
#ifndef REGOLITH_ARCHITECTURE_CLICKABLE_H_
#define REGOLITH_ARCHITECTURE_CLICKABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  /*
   * The class that defines an interface for anything that could act like a button.
   * It keeps track of its current state and has a call back function for a state change.
   */
  class Clickable : virtual public PhysicalObject
  {
    public: 
      enum State : char
      {
        NORMAL,
        FOCUSSED,
        DOWN,
        INACTIVE
      };

    private:
      State _state;

    protected:
      // For derived classes to handle state changes passes the new state, before updating the memebr variable
      // Therefore both the old and new states are available
      virtual void onStateChange( State ) = 0;

    public:
      Clickable() : _state( NORMAL ) {}

      virtual ~Clickable() {}


      // Configuration function
      virtual void configure( Json::Value& ) override;


      // This object is clickable
      virtual bool hasClick() const override { return true; }


      // Return the current state
      const State& currentState() const { return _state; }


      // Sets the current state to focussed if button is active
      void giveFocus();

      // Sets the current state to normal if button is active
      void takeFocus();

      // Returns false if the button is inactive
      void down(); 

      // Returns false if the button is inactive
      void up(); 

      // Allows the button to be clickable
      void activate();

      // Deactivates the button
      void deactivate();

  };

}

#endif // REGOLITH_ARCHITECTURE_CLICKABLE_H_


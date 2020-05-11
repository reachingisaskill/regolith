
#ifndef REOGLITH_GAMEPLAY_SIGNAL_H_
#define REOGLITH_GAMEPLAY_SIGNAL_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Components/Engine.h"


namespace Regolith
{

  /*
   * Signal base class.
   * Signals are are the intended method to attach events to elements in the Scene/Configuration files
   */

  // Forward declaration
  class Signal;
  class Context;

  // Function to interpret the json data and return a built signal pointer
  Signal* makeSignal( Json::Value&, Context* );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal Base class

  class Signal
  {
    private:

    public :
      // Con/Destruction
      Signal() {}
      virtual ~Signal() {}

      virtual void trigger() const = 0;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Action Signal

  class InputActionSignal : public Signal
  {
    private:
      InputAction _theAction;

    public :
      // Con/Destruction
      InputActionSignal( InputAction );
      virtual ~InputActionSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Boolean Action Signal

  class InputBooleanSignal : public Signal
  {
    private:
      InputAction _theAction;
      bool _theValue;

    public :
      // Con/Destruction
      InputBooleanSignal( InputAction, bool );
      virtual ~InputBooleanSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Float Action Signal

  class InputFloatSignal : public Signal
  {
    private:
      InputAction _theAction;
      float _theValue;

    public :
      // Con/Destruction
      InputFloatSignal( InputAction, float );
      virtual ~InputFloatSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vector Action Signal

  class InputVectorSignal : public Signal
  {
    private:
      InputAction _theAction;
      Vector _theValue;

    public :
      // Con/Destruction
      InputVectorSignal( InputAction, Vector );
      virtual ~InputVectorSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Mouse Action Signal

  class InputMouseSignal : public Signal
  {
    private:
      InputAction _theAction;
      Vector _position;
      bool _click;

    public :
      // Con/Destruction
      InputMouseSignal( InputAction, Vector, bool );
      virtual ~InputMouseSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Regolith Event Signal

  class GameEventSignal : public Signal
  {
    private:
      RegolithEvent _theEvent;

    public :
      // Con/Destruction
      GameEventSignal( RegolithEvent );
      virtual ~GameEventSignal() {}

      void trigger() const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Event Signal

//  class ContextEventSignal : public Signal
//  {
//    private:
//      ContextEvent _theEvent;
//
//    public :
//      // Con/Destruction
//      ContextEventSignal( ContextEvent );
//      virtual ~ContextEventSignal() {}
//
//      void trigger() const;
//  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Change Signal

  class ChangeContextSignal : public Signal
  {
    private:
      Context* _theContext;
      Engine::StackOperation::Operation _operation;

    public :
      // Con/Destruction
      ChangeContextSignal( Engine::StackOperation::Operation, Context* c = nullptr );
      virtual ~ChangeContextSignal() {}

      void trigger() const;
  };


}

#endif // REOGLITH_GAMEPLAY_SIGNAL_H_


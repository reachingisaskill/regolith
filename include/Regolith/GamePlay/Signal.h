
#ifndef REOGLITH_GAMEPLAY_SIGNAL_H_
#define REOGLITH_GAMEPLAY_SIGNAL_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/MassProduceable.h"


namespace Regolith
{

  /*
   * Signals.
   *
   * Signals provide a proxy interface to global operations that may be triggered by a context.
   *
   * For example: context and context group changes or input action simulation.
   */

  // Forward declaration
  class Signal;
  class Context;
  class ContextGroup;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal Base class

  class Signal
  {
    private:

    public :
      // Con/Destruction
      Signal() {}
      virtual ~Signal() {}

      virtual void configure( Json::Value&, ContextGroup& ) = 0;

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
      InputActionSignal();
      virtual ~InputActionSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
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
      InputBooleanSignal();
      virtual ~InputBooleanSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
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
      InputFloatSignal();
      virtual ~InputFloatSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
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
      InputVectorSignal();
      virtual ~InputVectorSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
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
      InputMouseSignal();
      virtual ~InputMouseSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Regolith Event Signal

  class GameEventSignal : public Signal
  {
    private:
      RegolithEvent _theEvent;

    public :
      // Con/Destruction
      GameEventSignal();
      virtual ~GameEventSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Change Signal

  class OpenContextSignal : public Signal
  {
    private:
      Context** _theContext;

    public :
      // Con/Destruction
      OpenContextSignal();
      virtual ~OpenContextSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
  };



////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Stack Change Signal

  class OpenContextStackSignal : public Signal
  {
    private:
      Context** _theContext;

    public :
      // Con/Destruction
      OpenContextStackSignal();
      virtual ~OpenContextStackSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Group Change Signal

  class OpenContextGroupSignal : public Signal
  {
    private:
      ContextGroup* _theContextGroup;

    public :
      // Con/Destruction
      OpenContextGroupSignal();
      virtual ~OpenContextGroupSignal() {}

      void trigger() const;

      virtual void configure( Json::Value&, ContextGroup& ) override;
  };


}

#endif // REOGLITH_GAMEPLAY_SIGNAL_H_


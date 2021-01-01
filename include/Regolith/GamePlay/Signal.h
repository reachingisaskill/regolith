
#ifndef REOGLITH_GAMEPLAY_SIGNAL_H_
#define REOGLITH_GAMEPLAY_SIGNAL_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/MassProduceable.h"
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
  class ContextGroup;
  class DataHandler;

//  // Function to interpret the json data and return a built signal pointer
//  Signal* makeSignal( Json::Value&, Context* );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal Base class

  class Signal : public MassProduceable<ContextGroup&>
  {
    private:

    public :
      // Con/Destruction
      Signal() {}
      virtual ~Signal() {}

      virtual void trigger() const = 0;

      virtual Signal* clone() const = 0;
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

      virtual Signal* clone() const override { return new InputActionSignal( *this ); }
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

      virtual Signal* clone() const override { return new InputBooleanSignal( *this ); }
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

      virtual Signal* clone() const override { return new InputFloatSignal( *this ); }
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

      virtual Signal* clone() const override { return new InputVectorSignal( *this ); }
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

      virtual Signal* clone() const override { return new InputMouseSignal( *this ); }
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

      virtual Signal* clone() const override { return new GameEventSignal( *this ); }
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

      virtual Signal* clone() const override { return new OpenContextSignal( *this ); }
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

      virtual Signal* clone() const override { return new OpenContextStackSignal( *this ); }
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

      virtual Signal* clone() const override { return new OpenContextGroupSignal( *this ); }
  };


}

#endif // REOGLITH_GAMEPLAY_SIGNAL_H_


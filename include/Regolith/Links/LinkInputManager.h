
#ifndef REGOLITH_LINKS_LINK_INPUT_MANAGER_H_
#define REGOLITH_LINKS_LINK_INPUT_MANAGER_H_

#include "Regolith/Links/Link.h"
#include "Regolith/Managers/InputManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< InputManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the input manager" );
  };


  
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handler access
  class InputHandler;

  template <>
  class Link< InputManager, InputHandler >
  {
    private:

      InputManager& _manager;

    public:

      Link( InputManager& m ) : _manager( m ) {}

      InputMappingSet* requestMapping( std::string m ) { return _manager.requestMapping( m ); }
  };

  
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal access
  class Signal;

  template <>
  class Link< InputManager, Signal >
  {
    private:

      InputManager& _manager;

    public:

      Link( InputManager& m ) : _manager( m ) {}

      void simulateInputAction( InputAction ia ) { _manager.simulateInputAction( ia ); }
      void simulateBooleanAction( InputAction ia , bool b ) { _manager.simulateBooleanAction( ia, b ); }
      void simulateFloatAction( InputAction ia, float f ) { _manager.simulateFloatAction( ia, f ); }
      void simulateVectorAction( InputAction ia, const Vector& v ) { _manager.simulateVectorAction( ia, v ); }
      void simulateMouseAction( InputAction ia, bool val, const Vector& pos ) { _manager.simulateMouseAction( ia, val, pos ); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine Manager access
  class EngineManager;

  template <>
  class Link< InputManager, EngineManager >
  {
    private:

      InputManager& _manager;

    public:

      Link( InputManager& m ) : _manager( m ) {}

      void handleEvents( InputHandler* h ) { _manager.handleEvents( h ); }
  };

}

#endif // REGOLITH_LINKS_LINK_INPUT_MANAGER_H_


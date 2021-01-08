
#ifndef REGOLITH_LINKS_LINK_WINDOW_H_
#define REGOLITH_LINKS_LINK_WINDOW_H_

#include "Regolith/Links/Link.h"
#include "Regolith/Managers/WindowManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< WindowManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the engine" );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rendering access
  struct EngineRenderingThreadType;

  template <>
  class Link< WindowManager, EngineRenderingThreadType >
  {
    private:

      WindowManager& _window;

    public:

      Link( WindowManager& m ) : _window( m ) {}

      Camera& create() { return _window.create(); }
  };

}

#endif // REGOLITH_LINKS_LINK_WINDOW_H_

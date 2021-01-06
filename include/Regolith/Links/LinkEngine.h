
#ifndef REGOLITH_LINKS_LINK_ENGINE_H_
#define REGOLITH_LINKS_LINK_ENGINE_H_

#include "Regolith/Links/Link.h"
#include "Regolith/Components/Engine.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< Engine, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the engine" );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rendering access
  struct EngineRenderingThreadType;

  template <>
  class Link< Engine, EngineRenderingThreadType >
  {
    private:

      Engine& _engine;

    public:

      Link( Engine& m ) : _engine( m ) {}

      ContextStack::reverse_iterator& visibleStackStart() { return _engine._visibleStackStart; }
      ContextStack::reverse_iterator& visibleStackEnd() { return _engine._visibleStackEnd; }

      std::mutex& renderMutex() { return _engine._renderMutex; }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal access
  class Signal;

  template <>
  class Link< Engine, Signal >
  {
    private:

      Engine& _engine;

    public:

      Link( Engine& m ) : _engine( m ) {}

      void openContext( Context* c ) { _engine.openContext( c ); }
      void openContextStack( Context* c ) { _engine.openContextStack( c ); }
      void openContextGroup( ContextGroup* cg ) { _engine.openContextGroup( cg ); }

  };
}

#endif // REGOLITH_LINKS_LINK_ENGINE_H_


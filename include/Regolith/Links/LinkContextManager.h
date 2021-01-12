
#ifndef REGOLITH_LINKS_LINK_CONTEXT_MANAGER_H_
#define REGOLITH_LINKS_LINK_CONTEXT_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/Link.h"
#include "Regolith/Managers/ContextManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< ContextManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the context manager" );
  };

  
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rendering thread access
  template <>
  class Link< ContextManager, EngineRenderingThreadType >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      void renderContextGroup( Camera& c ) { _manager.renderContextGroup( c ); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine Manager access
  class EngineManager;

  template <>
  class Link< ContextManager, EngineManager >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      void loadContextGroup( ContextGroup* cg ) { _manager.loadContextGroup( cg ); }
      void unloadContextGroup( ContextGroup* cg ) { _manager.unloadContextGroup( cg ); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context access
  class Context;
  class ContextGroup;

  template <>
  class Link< ContextManager, Context >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      ContextGroup* getContextGroup( std::string s ) { return _manager.getContextGroup( s ); }
      ContextGroup* getGlobalContextGroup() { return _manager.getGlobalContextGroup(); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context group access
  template <>
  class Link< ContextManager, ContextGroup >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      ContextGroup* getContextGroup( std::string s ) { return _manager.getContextGroup( s ); }
      ContextGroup* getGlobalContextGroup() { return _manager.getGlobalContextGroup(); }

      void requestRenderContextGroup( ContextGroup* cg ) { _manager.requestRenderContextGroup( cg ); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal access
  class Signal;

  template <>
  class Link< ContextManager, Signal >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      ContextGroup* getContextGroup( std::string s ) { return _manager.getContextGroup( s ); }
      ContextGroup* getGlobalContextGroup() { return _manager.getGlobalContextGroup(); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading thread access
  struct ContextManagerThreadType;

  template <>
  class Link< ContextManager, ContextManagerThreadType >
  {
    private:

      ContextManager& _manager;

    public:

      Link( ContextManager& m ) : _manager( m ) {}

      std::condition_variable& loadingThreadCondition() { return  _manager._loadingThreadCondition; }
      ContextManager::ContextGroupBuffer& contextGroupBuffer() { return _manager._contextGroupBuffer; }
      std::mutex& loadingThreadActive() { return _manager._loadingThreadActive; }
  };

}

#endif // REGOLITH_LINKS_LINK_CONTEXT_MANAGER_H_


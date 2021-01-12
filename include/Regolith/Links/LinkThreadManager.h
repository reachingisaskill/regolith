
#ifndef REGOLITH_LINKS_LINK_THREAD_MANAGER_H_
#define REGOLITH_LINKS_LINK_THREAD_MANAGER_H_

#include "Regolith/Links/Link.h"
#include "Regolith/Managers/ThreadManager.h"


namespace Regolith
{

  // Prevent partial specialisations from passing
  template < class REQUESTER >
  class Link< ThreadManager, REQUESTER >
  {
    struct FalseType : public std::false_type {};
    static_assert( FalseType::value, "A link is not permitted between the requested class and the thread manager" );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // ThreadHandler access
  class ThreadHandler;

  template <>
  class Link< ThreadManager, ThreadHandler >
  {
    private:

      ThreadManager& _manager;

    public:

      Link( ThreadManager& m ) : _manager( m ) {}

      void setThreadStatus( ThreadName n, ThreadStatus s ) { _manager.setThreadStatus( n, s ); }
      void waitThreadStatus( ThreadStatus s ) { _manager.waitThreadStatus( s ); }
      void waitThreadStatus( ThreadName n, ThreadStatus s ) { _manager.waitThreadStatus( n, s ); }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // ContextManager access
  class ContextManager;

  template <>
  class Link< ThreadManager, ContextManager >
  {
    private:

      ThreadManager& _manager;

    public:

      Link( ThreadManager& m ) : _manager( m ) {}

      void registerCondition( std::condition_variable* cv ) { _manager.registerCondition( cv ); }
  };

}

#endif // REGOLITH_LINKS_LINK_THREAD_MANAGER_H_



#ifndef REGOLITH_MANAGERS_CONTEXT_HANDLER_H_
#define REGOLITH_MANAGERS_CONTEXT_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Utilities/NamedVector.h"


namespace Regolith
{
  class DataHandler;
  class Context;

  class ContextHandler
  {
    friend class ContextManager;

    typedef std::set< IDNumber > DataCacheList;
    typedef std::set< IDNumber > ContextCacheList;

    private:
      // ID of this handler
      const IDNumber _id;

      // List of the data handles ids used by these contexts
      DataCacheList _dataCache;

      // List of the contexts controlled by this handler
      ContextCacheList _contextCache;

      //Reference to the global list
      NamedVector< Context, true >& _contexts;

    protected:
      // Constructor
      ContextHandler( IDNumber, NamedVector< Context, true >& );

    public:
      // Destructor
      ~ContextHandler();


      // Configure the handler
      void configure( Json::Value& );

      // Configures a context's data handler and caches the id number
      void configureDataHandler( DataHandler&, std::string );


      // Asks the Handler to build a context and store the memory. Returns a pointer to the new context
      Context* buildContext( Json::Value& );

  };

}

#endif // REGOLITH_MANAGERS_CONTEXT_HANDLER_H_


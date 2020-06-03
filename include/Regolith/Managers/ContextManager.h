
#ifndef REGOLITH_MANAGERS_CONTEXT_MANAGER_H_
#define REGOLITH_MANAGERS_CONTEXT_MANAGER_H_

#include "Regolith/Managers/ContextHandler.h"
#include "Regolith/Utilities/NamedReferenceVector.h"


namespace Regolith
{

  class ContextManager
  {
    private:
      // Owns the memory describing every context
      NamedVector< Context, true > _contexts;

      // Vector of the individual context handlers
      NamedVector< ContextHandler, true > _contextHandlers;


    public:
      // Con/Destructors
      ContextManager();

      ~ContextManager();

      // Configure the manager
      void configure( Json::Value& );

      // Validate the created contexts
      void validate() const;

//////////////////////////////////////////////////////////////////////////////// 
      // Context construction/manipulation

      // Return a pointer to a requested context - creating an entry if one doesn't exist
      IDNumber requestContext( std::string name ) { return _contexts.addName( name ); }

      // Return the name of a given context. Mostly useful for debugging!
      std::string getContextName( IDNumber id ) { return _contexts.getName( id ); }

      // Return a pointer to a requested context
      Context* getContext( IDNumber id ) { return _contexts[ id ]; }

  };

}

#endif // REGOLITH_MANAGERS_CONTEXT_MANAGER_H_


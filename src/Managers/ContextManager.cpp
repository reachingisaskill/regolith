
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ContextManager::ContextManager() :
    _contexts( "manager_context_list" ),
    _contextHandlers( "manager_context_handlers" )
  {
    _contexts.addObject( nullptr, "null" );
    _contextHandlers.addObject( nullptr, "null" );
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "Destroying Context Manager" );
    _contexts.clear();
    _contextHandlers.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& context_data )
  {
    Json::ArrayIndex context_data_size = context_data.size();
    for ( Json::ArrayIndex i = 0; i != context_data_size; ++i )
    {
      Utilities::validateJson( context_data[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = context_data[i]["name"].asString();

      INFO_STREAM << "Bulding context handler: " << name;

      IDNumber id = _contextHandlers.addObject( new ContextHandler( _contextHandlers.size(), _contexts ), name );

      _contextHandlers[ id ]->configure( context_data[i] );
    }
  }


  void ContextManager::validate() const
  {
    NamedVector<Context, true>::const_iterator context_end = _contexts.end();
    for ( NamedVector<Context, true>::const_iterator it = _contexts.begin(); it != context_end; ++it )
    {
      if ( (*it) != nullptr )
        (*it)->validate();
    }
  }

}


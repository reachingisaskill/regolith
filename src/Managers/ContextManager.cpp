
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Architecture/Context.h"


namespace Regolith
{

  ContextManager::ContextManager() :
    _contexts( "manager_context_list" ),
    _contextHandlers()
  {
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "Destroying Context Manager" );
    _contexts.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& context_data )
  {
    Json::ArrayIndex context_data_size = context_data.size();
    _contextHandlers.reserve( context_data_size );

    for ( Json::ArrayIndex i = 0; i != context_data_size; ++i )
    {
      INFO_LOG( "Bulding context handler" );

      _contextHandlers.push_back( ContextHandler( _contextHandlers.size(), _contexts ) );

      _contextHandlers[ _contextHandlers.size()-1 ].buildContext( context_data[i] );
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


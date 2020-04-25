
#include "Utilities.h"
#include "Exception.h"

#include "logtastic.h"


namespace Regolith
{

  namespace Utilities
  {

    void validateJson( Json::Value& json_data, const char* name, JsonType type )
    {
      if ( ! json_data.isMember( name ) )
      {
        FAILURE_STREAM << "Json data failed validation. Expected key: " << name;
        Exception ex( "validateJson()", "Key not found", false );
        ex.addDetail( "Key name", name );
        throw ex;
      }

      if ( type != JSON_TYPE_NULL )
      {

        switch ( type )
        {
          case JSON_TYPE_OBJECT :
            if ( json_data[ name ].isObject() )
              return;
            break;
          case JSON_TYPE_ARRAY :
            if ( json_data[ name ].isArray() )
              return;
            break;
          case JSON_TYPE_STRING :
            if ( json_data[ name ].isString() )
              return;
            break;
          case JSON_TYPE_FLOAT :
            if ( json_data[ name ].isNumeric() )
              return;
            break;
          case JSON_TYPE_INTEGER :
            if ( json_data[ name ].isIntegral() )
              return;
            break;
          default :
            return;
            break;
        }

        FAILURE_STREAM << "Json data failed validation. Could not resolve type ID: " << type;
        Exception ex( "validateJson()", "Wong data type", false );
        ex.addDetail( "Name", name );
        ex.addDetail( "Type ID", type );
        throw ex;

      }
    }

  }
}


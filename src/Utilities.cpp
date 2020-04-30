
#include "Utilities.h"
#include "Exception.h"

#include "logtastic.h"


namespace Regolith
{

  namespace Utilities
  {

    bool validateJson( Json::Value& json_data, const char* name, JsonType type, bool isRequired )
    {
      if ( ! json_data.isMember( name ) )
      {
        if ( isRequired )
        {
          FAILURE_STREAM << "Json data failed validation. Expected key: " << name;
          Exception ex( "validateJson()", "Key not found", false );
          ex.addDetail( "Key name", name );
          throw ex;
        }
        else
        {
          return false;
        }
      }

      if ( type != JSON_TYPE_NULL )
      {

        switch ( type )
        {
          case JSON_TYPE_OBJECT :
            if ( json_data[ name ].isObject() )
              return true;
            break;
          case JSON_TYPE_ARRAY :
            if ( json_data[ name ].isArray() )
              return true;
            break;
          case JSON_TYPE_STRING :
            if ( json_data[ name ].isString() )
              return true;
            break;
          case JSON_TYPE_FLOAT :
            if ( json_data[ name ].isNumeric() )
              return true;
            break;
          case JSON_TYPE_INTEGER :
            if ( json_data[ name ].isIntegral() )
              return true;
            break;
          case JSON_TYPE_BOOLEAN :
            if ( json_data[ name ].isBool() )
              return true;
            break;
          default :
            return true;
            break;
        }

        FAILURE_STREAM << "Json data failed validation. Could not resolve type ID: " << type;
        Exception ex( "validateJson()", "Wong data type", false );
        ex.addDetail( "Name", name );
        ex.addDetail( "Type ID", type );
        throw ex;
      }
      return true;
    }


    void validateJsonArray( Json::Value& json_data, unsigned int length, JsonType type )
    {
      if ( ! json_data.isArray() )
      {
        FAILURE_LOG( "Json data failed validation. Expected array type" );
        Exception ex( "validateJsonArray()", "Json object is not an array", false );
        throw ex;
      }

      Json::ArrayIndex size = json_data.size();
      if ( size < length )
      {
        FAILURE_STREAM << "Json data failed validation. Expected array of at least size : " << length << " Found size : " << size;
        Exception ex( "validateJsonArray()", "Array is incorrect size", false );
        ex.addDetail( "Expected", length );
        ex.addDetail( "Found", size );
        throw ex;
      }

      bool success = true;

      if ( type != JSON_TYPE_NULL )
      {
        for ( Json::ArrayIndex i = 0; i < length; ++i )
        {
          switch ( type )
          {
            case JSON_TYPE_OBJECT :
              if ( ! json_data[i].isObject() )
                return;
              break;
            case JSON_TYPE_ARRAY :
              if ( ! json_data[i].isArray() )
                return;
              break;
            case JSON_TYPE_STRING :
              if ( ! json_data[i].isString() )
                return;
              break;
            case JSON_TYPE_FLOAT :
              if ( ! json_data[i].isNumeric() )
                return;
              break;
            case JSON_TYPE_INTEGER :
              if ( ! json_data[i].isIntegral() )
                return;
              break;
            default :
              return;
              break;
          }
        }
      }

      if ( ! success )
      {
        FAILURE_STREAM << "Json data failed validation. Expected array of type: " << type;
        Exception ex( "validateJsonArray()", "Wong data type", false );
        ex.addDetail( "Type ID", type );
        throw ex;
      }
    }

  }
}



#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  bool validateJson( Json::Value& json_data, const char* name, JsonType type, bool isRequired )
  {
    if ( ! json_data.isMember( name ) || json_data[name].isNull() )
    {
      if ( isRequired )
      {
        ERROR_STREAM << "Utilities::validateJson : Json data failed validation. Expected key: " << name;
        Exception ex( "validateJson()", "Key not found", false );
        ex.addDetail( "Key name", name );
        throw ex;
      }
      else
      {
        return false;
      }
    }

    try
    {
      validateJson( json_data[name], type );
    }
    catch ( Exception& e )
    {
      e.addDetail( "Name", name );
      throw e;
    }

    return true;
  }


  void validateJson( Json::Value& json_data, JsonType type )
  {
    if ( type != JsonType::NONE )
    {

      switch ( type )
      {
        case JsonType::OBJECT :
          if ( json_data.isObject() )
            return;
          break;
        case JsonType::ARRAY :
          if ( json_data.isArray() )
            return;
          break;
        case JsonType::STRING :
          if ( json_data.isString() )
            return;
          break;
        case JsonType::FLOAT :
          if ( json_data.isNumeric() )
            return;
          break;
        case JsonType::INTEGER :
          if ( json_data.isIntegral() )
            return;
          break;
        case JsonType::BOOLEAN :
          if ( json_data.isBool() )
            return;
          break;
        default :
          return;
          break;
      }

      ERROR_STREAM << "Utilities::validateJson : Json data failed validation. Could not resolve type ID: " << static_cast<int>( type );
      Exception ex( "validateJson()", "Wong data type", false );
      ex.addDetail( "Type ID", static_cast<int>( type ) );
      throw ex;
    }
  }


  void validateJsonArray( Json::Value& json_data, unsigned int length, JsonType type )
  {
    if ( ! json_data.isArray() )
    {
      ERROR_LOG( "Utilities::validateJsonArray : Json data failed validation. Expected array type" );
      Exception ex( "validateJsonArray()", "Json object is not an array", false );
      throw ex;
    }

    Json::ArrayIndex size = json_data.size();
    if ( size < length )
    {
      ERROR_STREAM << "Utilities::validateJsonArray : Json data failed validation. Expected array of at least size : " << length << " Found size : " << size;
      Exception ex( "validateJsonArray()", "Array is incorrect size", false );
      ex.addDetail( "Expected", length );
      ex.addDetail( "Found", size );
      throw ex;
    }

    bool success = true;

    if ( type != JsonType::NONE )
    {
      for ( Json::ArrayIndex i = 0; i < length; ++i )
      {
        switch ( type )
        {
          case JsonType::OBJECT :
            if ( ! json_data[i].isObject() )
              return;
            break;
          case JsonType::ARRAY :
            if ( ! json_data[i].isArray() )
              return;
            break;
          case JsonType::STRING :
            if ( ! json_data[i].isString() )
              return;
            break;
          case JsonType::FLOAT :
            if ( ! json_data[i].isNumeric() )
              return;
            break;
          case JsonType::INTEGER :
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
      ERROR_STREAM << "Utilities::validateJsonArray : Json data failed validation. Expected array of type: " << static_cast<int>( type );
      Exception ex( "validateJsonArray()", "Wong data type", false );
      ex.addDetail( "Type ID", static_cast<int>( type ) );
      throw ex;
    }
  }


  /*
  void jsonProcessPosition( Json::Value& json_data, Drawable* object, Camera* camera )
  {
    float x = 0.0;
    float y = 0.0;

    if ( validateJson( json_data, "position", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["position"], 2, JsonType::FLOAT );

      x = json_data["position"][0].asFloat();
      y = json_data["position"][1].asFloat();
    }


    if ( validateJson( json_data, "alignment", JsonType::OBJECT, false ) )
    {
      if ( validateJson( json_data["alignment"], "horizontal", JsonType::STRING, false ) )
      {
        if ( json_data["alignment"]["horizontal"].asString() == "centre" )
        {
          x += (float)camera->getWidth()/2 - (float)object->getWidth()/2;
        }
        else if ( json_data["alignment"]["horizontal"].asString() == "left" )
        {
          // Do nothing - this is the default
        }
        else if ( json_data["alignment"]["horizontal"].asString() == "right" )
        {
          x = ( (float)camera->getWidth() - (float)object->getWidth() ) - x;
        }
      }
      if ( validateJson( json_data["alignment"], "vertical", JsonType::STRING, false ) )
      {
        if ( json_data["alignment"]["vertical"].asString() == "centre" )
        {
          y += (float)camera->getHeight()/2 - (float)object->getHeight()/2;
        }
        else if ( json_data["alignment"]["horizontal"].asString() == "top" )
        {
          // Do nothing - this is the default
        }
        else if ( json_data["alignment"]["horizontal"].asString() == "bottom" )
        {
          y = ( (float)camera->getHeight() - (float)object->getHeight() ) - y;
        }
      }
    }

    Vector vec( x, y );
    INFO_STREAM << "Utilities::jsonProcessPosition : Calculated object position to be: " << vec;
    object->setPosition( vec );
  }
  */


  void loadJsonData( Json::Value& json_data, std::string filename )
  {
    INFO_STREAM << "Utilities::loadJsonData : Loading Json File: " << filename;
    std::ifstream input( filename );
    Json::CharReaderBuilder reader_builder;
    Json::CharReader* reader = reader_builder.newCharReader();
    std::string errors;
    bool result = Json::parseFromStream( reader_builder, input, &json_data, &errors );
    if ( ! result )
    {
      ERROR_STREAM << "Utilties::loadJsonData() : Found errors parsing json file: " << filename;
      ERROR_STREAM << errors;
    }
    delete reader;
  }

}


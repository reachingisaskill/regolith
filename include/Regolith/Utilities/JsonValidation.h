
#ifndef REGOLITH_UTILITIES_JSON_VALIDATION_H_
#define REGOLITH_UTILITIES_JSON_VALIDATION_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  class Drawable;
  class Camera;

  namespace Utilities
  {
    enum JsonType
    {
      JSON_TYPE_NULL,
      JSON_TYPE_OBJECT,
      JSON_TYPE_ARRAY,
      JSON_TYPE_STRING,
      JSON_TYPE_FLOAT,
      JSON_TYPE_INTEGER,
      JSON_TYPE_BOOLEAN,
      JSON_TYPE_TOTAL
    };

    bool validateJson( Json::Value&, const char*, JsonType type = JSON_TYPE_NULL, bool required = true );

    void validateJsonArray( Json::Value&, unsigned int, JsonType type = JSON_TYPE_NULL );

    void jsonProcessPosition( Json::Value&, Drawable*, Camera* );

    void loadJsonData( Json::Value&, std::string );

  }
}

#endif // REGOLITH_UTILITIES_JSON_VALIDATION_H_


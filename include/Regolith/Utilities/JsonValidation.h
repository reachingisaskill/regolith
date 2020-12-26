
#ifndef REGOLITH_UTILITIES_JSON_VALIDATION_H_
#define REGOLITH_UTILITIES_JSON_VALIDATION_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  class Drawable;
  class Camera;

  enum class JsonType
  {
    NONE,
    OBJECT,
    ARRAY,
    STRING,
    FLOAT,
    INTEGER,
    BOOLEAN,
    TOTAL
  };

  bool validateJson( Json::Value&, const char*, JsonType type = JsonType::NONE, bool required = true );

  void validateJson( Json::Value&, JsonType type = JsonType::NONE );

  void validateJsonArray( Json::Value&, unsigned int, JsonType type = JsonType::NONE );

  void jsonProcessPosition( Json::Value&, Drawable*, Camera* );

  void loadJsonData( Json::Value&, std::string );

}

#endif // REGOLITH_UTILITIES_JSON_VALIDATION_H_


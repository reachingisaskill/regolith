
#ifndef __REGOLITH__UTILITIES_H__
#define __REGOLITH__UTILITIES_H__

#include "Definitions.h"

#include <json/json.h>

namespace Regolith
{

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
      JSON_TYPE_TOTAL
    };

    void validateJson( Json::Value&, const char*, JsonType type = JSON_TYPE_NULL );

  }
}

#endif // __REGOLITH__UTILITIES_H__


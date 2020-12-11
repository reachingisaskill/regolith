
#include "Regolith/GamePlay/Text.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <fstream>


namespace Regolith
{

  std::string* loadText( std::string filename )
  {
    std::ifstream reader( filename );

    std::string* data = new std::string();
    data->assign( std::istreambuf_iterator<char>( reader ), std::istreambuf_iterator<char>() );

    return data;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  Text::Text() :
    _rawText( nullptr ),
    _horizontalAlignment( ALIGNMENT_LOW_EDGE ),
    _verticalAlignment( ALIGNMENT_LOW_EDGE )
  {
  }


  Text::~Text()
  {
  }


  void Text::configure( Json::Value& json_data, DataHandler& handler )
  {
    Utilities::validateJson( json_data, "text_name", Utilities::JSON_TYPE_STRING );

    _rawText = handler.getRawText( json_data["text_name"].asString() );
  }

}


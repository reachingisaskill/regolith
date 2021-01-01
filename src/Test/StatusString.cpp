
#include "Regolith/Test/StatusString.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  StatusString::StatusString() :
    _texture(),
    _string()
  {
  }


  StatusString::~StatusString()
  {
  }


  void StatusString::configure( Json::Value& json_data, ContextGroup& cg )
  {
    PhysicalObject::configure( json_data, cg );

    _texture.configure( json_data, cg.getDataHandler() );

    if ( validateJson( json_data, "text", JsonType::STRING, false ) )
    {
      std::string text = json_data["text"].asString();
      this->setStatus( text );
      DEBUG_STREAM << "StatusString::configure : Writing text : " << text;
    }
    else
    {
      this->setWidth( _texture.getWidth() );
      this->setHeight( _texture.getHeight() );
    }

    DEBUG_LOG( "StatusString::configure : Status string object configured" );
  }


  void StatusString::setStatus( std::string new_text )
  {
    _string = new_text;

    _texture.writeText( _string );
    this->setWidth( _texture.getWidth() );
    this->setHeight( _texture.getHeight() );
  }

}


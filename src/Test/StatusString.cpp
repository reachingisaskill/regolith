
#include "Regolith/Test/StatusString.h"
#include "Regolith/Managers/ContextGroup.h"


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

    this->setWidth( _texture.getWidth() );
    this->setHeight( _texture.getHeight() );

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


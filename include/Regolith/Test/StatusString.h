
#ifndef REGOLITH_TEST_STATUS_STRING_H_
#define REGOLITH_TEST_STATUS_STRING_H_

#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/Textures/ShortText.h"


namespace Regolith
{

  class StatusString : public DrawableObject
  {
    private:
      ShortText _texture;

      std::string _string;


    public:
      StatusString();

      virtual ~StatusString();


      // Clone this class
      virtual PhysicalObject* clone() const { return (PhysicalObject*) new StatusString( *this ); }

      // Update the text
      void setStatus( std::string );

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;


      // For the camera to request the current renderable texture
      virtual Texture& getTexture() { return _texture; }

  };

}

#endif // REGOLITH_TEST_STATUS_STRING_H_


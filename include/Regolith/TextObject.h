
#ifndef REGOLITH_TEXT_OBJECT_H_
#define REGOLITH_TEXT_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/PhysicalObject.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Text.h"
#include "Regolith/GamePlay/Font.h"


namespace Regolith
{

  class TextObject : public PhysicalObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Texture _texture;

      // Font object
      Font _font;

      // Text to display
      Text _text;

      // Collision for the object
      Collision _collision;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      TextObject( const TextObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      TextObject();

      // Destructor
      virtual ~TextObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual TextObject* clone() const { return new TextObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset() override;

////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // Perform an update to all the animations
      virtual void update( float timestep ) override;

      // For the camera to request the current renderable texture
      virtual const Texture& getTexture() const override;

      // For the collision handler to request the current hitboxes
      virtual const Collision& getCollision() const override;
  };

}

#endif // REGOLITH_TEXT_OBJECT_H_



#ifndef REGOLITH_GAMMEOBJECTS_SIMPLE_BUTTON_H_
#define REGOLITH_GAMMEOBJECTS_SIMPLE_BUTTON_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/Architecture/Clickable.h"
#include "Regolith/GamePlay/Texture.h"


namespace Regolith
{
  class Signal;

  class SimpleButton : public Drawable, public Interactable, public Clickable
  {
    private :
      Texture _textures[Clickable::STATE_MAX];
      SDL_Rect _destination;
      Texture* _currentTexture;

    protected:
      virtual void onStateChange( Clickable::State ) override;

    public :
      // Create in new button
      SimpleButton();

      // Destroy and clear the texture set
      virtual ~SimpleButton();

      // Configure the button textures
      virtual void configure( Json::Value&, DataHandler& ) override;


      // Return a new clone of the current object. Transfers ownership of memory
      virtual PhysicalObject* clone( const Vector& ) const override;


      // Render with the current renderer object
      virtual void render( const Camera& ) override;


      // Button click - trigger the interactions
      virtual void click() override { this->trigger(); }


      // Override the width/height getters
      virtual float getWidth() const override { return _currentTexture->getWidth(); }
      virtual float getHeight() const override { return _currentTexture->getHeight(); }
  };

}

#endif // REGOLITH_GAMMEOBJECTS_SIMPLE_BUTTON_H_


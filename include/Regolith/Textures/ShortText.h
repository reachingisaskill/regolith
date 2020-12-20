
#ifndef REGOLITH_TEXTURES_SHORT_TEXT_H_
#define REGOLITH_TEXTURES_SHORT_TEXT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Textures/Texture.h"
#include "Regolith/Assets/RawFont.h"


namespace Regolith
{

  // Class to hold short, editable pieces of text with very little bounds checking, etc.
  class ShortText : public Texture
  {
    private:
      RawFont* _rawFont;
//      RawText* _rawText;

      // Flag to signal the text has changed and must be re-rendered
      bool _update;

      SDL_Texture* _theTexture;
      SDL_Surface* _theSurface;
      SDL_Rect _clip;
      SDL_RendererFlip _flipFlag;
      SDL_Point _center;
      float _rotation;

      AlignmentMode _horizontalAlignment;
      AlignmentMode _verticalAlignment;

    protected:
      // Return a pointer the raw, SDL texture
      virtual SDL_Texture* getSDLTexture() override { return _theTexture; }

      // Return a pointer to the clip rect for the rendering process
      virtual SDL_Rect* getClip() override { return &_clip; }

      // Return the flip flag
      virtual SDL_RendererFlip getRendererFlip() override { return _flipFlag; }

      // Return a pointer to the center point (If one is defined)
      virtual SDL_Point* getTextureCenter() override { return &_center; }

      // Return the rotation value
      virtual double getRotation() override { return _rotation; }


      // Return a pointer to the surface to render
      virtual SDL_Surface* getUpdateSurface() override { return _theSurface; }

      // Set the newly rendered texture
      virtual void setRenderedTexture( SDL_Texture* ) override;


      // Clear the rendered texture
      virtual void clearSDLTexture() override;


    public:
      // Trivial constructor
      ShortText();

      // Virtual Destructor
      virtual ~ShortText() {}

      // Return true if a surface needs to be rendered during the rendering cycle
      virtual bool update() const { return _update; }

      // Force the derived classes to configure themselves
      virtual void configure( Json::Value&, DataHandler& ) override;

      // Use the font and default settings to write the text onto a surface and set the update flag
      void writeText( std::string& );

  };

}

#endif // REGOLITH_TEXTURES_SHORT_TEXT_H_


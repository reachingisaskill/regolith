
#ifndef __REGOLITH__FPS_STRING_H__
#define __REGOLITH__FPS_STRING_H__

#include "Definitions.h"

#include "Texture.h"
#include "Camera.h"

#include <sstream>


namespace Regolith
{

  class FPSString : public Drawable
  {
    private:
      RawTexture _theTexture;
      std::string _phrase;
      unsigned int _frameCounter;
      unsigned int _limit;
      Uint32 _tickCount;
      std::stringstream _stringstream;
      TTF_Font* _theFont;
      SDL_Color _theColor;
      SDL_Rect _destination;

    public:

      FPSString();

      FPSString( const FPSString& );

      virtual ~FPSString();

      // Specify the properties of the object. (Moving, animated, collision, etc)
      virtual bool hasCollision() const { return false; }
      virtual bool hasInput() const { return false; }
      virtual bool hasAnimation() const { return true; }

      // Perform the steps to call SDL_RenderCopy, etc
      virtual void render( Camera* );

      // Handle Events
      virtual void registerEvents( InputHandler* ) {}

      // Update the objects behaviour based on the provided timestep
      virtual void update( Uint32 );

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& ) { return 0; }

      // Add a collision object - reports and error for this data type
      virtual void addCollision( Collision* );

      // Return a clone of the current object. Transfers ownership of memory
      Drawable* clone() const;

      // Size accessors
      virtual int getWidth() const { return _theTexture.width; }
      virtual int getHeight() const { return _theTexture.height; }


      void setFrameCount( unsigned int n ) { _limit = n; }

      void setFont( TTF_Font* f ) { _theFont = f; }

      void setColor( SDL_Color c ) { _theColor = c; }

      void updateString();

  };

}

#endif // __REGOLITH__FPS_STRING_H__


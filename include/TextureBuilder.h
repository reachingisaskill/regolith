
#ifndef __REGOLITH__TEXTURE_BUILDER_H__
#define __REGOLITH__TEXTURE_BUILDER_H__

#include "Texture.h"
#include "SpriteSheet.h"

#include <SDL2/SDL.h>

#include <string>
#include <json/json.h>


namespace Regolith
{

  class TextureBuilder
  {
    private:
      SDL_Renderer* _theRenderer;

    protected:

      Texture* buildTexture( Texture*, Json::Value& );
      Texture* buildSpriteSheet( SpriteSheet*, Json::Value& );

    public:

      TextureBuilder( SDL_Renderer* );

      // Only needs a json object containing all the relevant info
      Texture* build( RawTexture, Json::Value );
  };

}

#endif // __REGOLITH__TEXTURE_BUILDER_H__


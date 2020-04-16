
#ifndef __REGOLITH__TEXTURE_BUILDER_H__
#define __REGOLITH__TEXTURE_BUILDER_H__

#include "Texture.h"

#include <string>

#include <json/json.h>


namespace Regolith
{

  enum TextureBuildType
  {
    TEXTURE_BUILD_TEXTURE,
    TEXTURE_BUILD_TEXT,
    TEXTURE_BUILD_SPRITESHEET,
    TEXTURE_BUILD_ANIMATEDSPRITE,
    TEXTURE_BUILD_TOTAL
  };

  class TextureBuilder
  {
    private:
      SDL_Renderer* _theRenderer;

    protected:

      Texture* buildTexture( Json::Value& );
      Texture* buildText( Json::Value& );
      Texture* buildSpriteSheet( Json::Value& );

    public:

      TextureBuilder( SDL_Renderer* );

      // Only needs a json object containing all the relevant info
      Texture* build( Json::Value );
  };

}

#endif // __REGOLITH__TEXTURE_BUILDER_H__


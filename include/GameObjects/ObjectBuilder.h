
#ifndef REGOLITH_TEXTURE_BUILDER_H_
#define REGOLITH_TEXTURE_BUILDER_H_

#include "Managers/Definitions.h"
#include "Architecture/Drawable.h"
#include "GameObjects/ObjectFactory.h"

#include <string>
#include <json/json.h>


namespace Regolith
{

  // Forward declarations
  class Scene;


  class ObjectBuilder
  {
    // Typedefs
    typedef std::map< std::string, ObjectFactory_base* > FactoryMap;

    private:
      FactoryMap _factories;
      SDL_Renderer* _theRenderer;

    protected:

    public:

      ObjectBuilder();

      explicit ObjectBuilder( SDL_Renderer* );

      virtual ~ObjectBuilder();

      // Configure for the current scene being loaded
      void setRenderer( SDL_Renderer* );

      SDL_Renderer* getRenderer() const { return this->_theRenderer; }

      // Add a new factory to the build system - takes ownership of data
      void addFactory( ObjectFactory_base* );

      // Only needs a json object containing all the relevant info
      Drawable* build( Json::Value& );
  };

}

#endif // REGOLITH_TEXTURE_BUILDER_H_


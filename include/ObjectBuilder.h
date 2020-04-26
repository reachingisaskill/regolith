
#ifndef __REGOLITH__TEXTURE_BUILDER_H__
#define __REGOLITH__TEXTURE_BUILDER_H__

#include "Definitions.h"

#include "Drawable.h"
#include "ObjectFactory.h"

#include <string>
#include <json/json.h>


namespace Regolith
{

  // Forward declarations
  class Scene;


  // Typedefs
  typedef std::map< std::string, ObjectFactory_base* > FactoryMap;



  class ObjectBuilder
  {
    private:
      FactoryMap _factories;
      SDL_Renderer* _theRenderer;
      const Scene* _theScene;

    protected:

    public:

      ObjectBuilder();

      explicit ObjectBuilder( SDL_Renderer* );

      virtual ~ObjectBuilder();

      // Configure for the current scene being loaded
      void setRenderer( SDL_Renderer* );
      void setScene( const Scene* );

      SDL_Renderer* getRenderer() const { return this->_theRenderer; }

      const Scene* getScene() const { return _theScene; }

      // Add a new factory to the build system - takes ownership of data
      void addFactory( ObjectFactory_base* );

      // Only needs a json object containing all the relevant info
      Drawable* build( Json::Value );
  };

}

#endif // __REGOLITH__TEXTURE_BUILDER_H__

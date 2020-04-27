
#ifndef __REGOLITH__SCENE_BUILDER_H__
#define __REGOLITH__SCENE_BUILDER_H__

#include "Definitions.h"

#include <json/json.h>

#include <string>


namespace Regolith
{

  // Forward defines
  class SceneFactory_base;
  class Window;
  class ObjectBuilder;
  class Scene;

  // Typedefs
  typedef std::map< std::string, SceneFactory_base* > SceneFactoryMap;

////////////////////////////////////////////////////////////////////////////////////////////////////

  class SceneFactory_base
  {
    private :
      std::string _name;

    public :
      SceneFactory_base( std::string n ) : _name( n ) {}

      virtual ~SceneFactory_base() {}

      std::string getSceneName() const { return _name; }

      virtual Scene* build( std::string ) const = 0;
  };


  template < class SCENE >
  class SceneFactory : public SceneFactory_base
  {
    private :
    public :

      SceneFactory( std::string n ) : SceneFactory_base( n ) {}
      virtual ~SceneFactory() {}

      virtual Scene* build( std::string ) const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////

  class SceneBuilder
  {
    private:
      SDL_Renderer* _theRenderer;
      Window* _window;
      ObjectBuilder* _objectBuilder;
      SceneFactoryMap _factories;

    protected:

    public:
      SceneBuilder();

      SceneBuilder( SDL_Renderer*, Window*, ObjectBuilder* );

      virtual ~SceneBuilder();

      SDL_Renderer* getRenderer() const { return this->_theRenderer; }

      void addFactory( SceneFactory_base* );

      // Only needs a json object containing all the relevant info
      Scene* build( Json::Value& );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template member function definitions

  template < class SCENE >
  Scene* SceneFactory<SCENE>::build( std::string filename ) const
  {
    return new SCENE( filename );
  }

}

#endif // __REGOLITH__SCENE_BUILDER_H__




#ifndef REGOLITH_SCENE_BUILDER_H_
#define REGOLITH_SCENE_BUILDER_H_

#include "Definitions.h"

#include <json/json.h>

#include <string>


namespace Regolith
{

  // Forward defines
  class SceneFactory;
  class Window;
  class ObjectBuilder;
  class Scene;

  // Typedefs
  typedef std::map< std::string, SceneFactory* > SceneFactoryMap;

////////////////////////////////////////////////////////////////////////////////////////////////////

  class SceneFactory
  {
    private :

    protected :
      void configureTeams( Scene* ) const;
      void buildSounds( Scene*, Json::Value& ) const;
      void buildBackground( Scene*, Json::Value& ) const;
      void buildCameras( Scene*, Json::Value& ) const;
      void buildEnvironment( Scene*, Json::Value& ) const;
      void buildHUD( Scene*, Json::Value& ) const;
      void buildDialogs( Scene*, Json::Value& ) const;
      void buildOptions( Scene*, Json::Value& ) const;

    public :
      SceneFactory() {}

      virtual ~SceneFactory() {}

      virtual const char* getSceneName() const = 0;

      virtual Scene* build( Json::Value& ) const = 0;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////

  class SceneBuilder
  {
    private:
      SceneFactoryMap _factories;

    protected:

    public:
      SceneBuilder();

      virtual ~SceneBuilder();

      void addFactory( SceneFactory* );

      // Only needs a json object containing all the relevant info
      Scene* build( Json::Value& );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Scene Factories

//////////////////////////////////////////////////
  // Title Scene

  class TitleSceneFactory : public SceneFactory
  {
    private :
    public :
      TitleSceneFactory() {}

      virtual ~TitleSceneFactory() {}

      virtual const char* getSceneName() const override { return "title"; }

      virtual Scene* build( Json::Value& ) const override;
  };


//////////////////////////////////////////////////
  // Platformer Scene

  class PlatformerSceneFactory : public SceneFactory
  {
    private :
    public :
      PlatformerSceneFactory() {}

      virtual ~PlatformerSceneFactory() {}

      virtual const char* getSceneName() const override { return "platformer"; }

      virtual Scene* build( Json::Value& ) const override;
  };

}

#endif // REGOLITH_SCENE_BUILDER_H_



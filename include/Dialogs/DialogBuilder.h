
#ifndef REGOLITH_DIALOG_BUILDER_H_
#define REGOLITH_DIALOG_BUILDER_H_

#include "Managers/Definitions.h"

#include <json/json.h>


namespace Regolith
{
  class Camera;

  class DialogBuilder;
  class DialogFactory;
  class Dialog;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Factory - base class

  class DialogFactory
  {
    friend class DialogBuilder;

    private :
      DialogBuilder* _parent;

    protected :
      void buildBackground( Dialog*, Camera*, Json::Value& ) const;

      void buildElements( Dialog*, Camera*, Json::Value& ) const;

      void buildOptions( Dialog*, Camera*, Json::Value& ) const;

    public:
      DialogFactory() : _parent( nullptr ) {}
      virtual ~DialogFactory() {}

      DialogBuilder* getParent() const { return _parent; }
      void setParent( DialogBuilder* p ) { _parent = p; }

      virtual const char* getDialogName() const = 0;

      virtual Dialog* build( Json::Value&, Camera* ) const = 0;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dialog Builder

  class DialogBuilder
  {
    DialogBuilder( const DialogBuilder& ) = delete;
    DialogBuilder& operator=( const DialogBuilder& ) = delete;

    typedef std::map< std::string, DialogFactory* > FactoryMap;

    private:
      FactoryMap _factories;

    public:

      DialogBuilder() {}

      ~DialogBuilder();

      void addFactory( DialogFactory* );

      Dialog* build( Json::Value&, Camera* ) const;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Factory Classes

//////////////////////////////////////////////////
  // Menu Factory
  class MenuDialogFactory : public DialogFactory
  {
    private:

    public:
      MenuDialogFactory() {}

      const char* getDialogName() const override { return "menu"; }

      Dialog* build( Json::Value&, Camera* ) const override;
  };


}

#endif // REGOLITH_DIALOG_BUILDER_H_


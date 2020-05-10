
#ifndef REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_
#define REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_

#include "Regolith/Global/Json.h"
#include "Regolith/Architecture/MassProduceable.h"


namespace Regolith
{
  namespace Architecture
  {

    // Base Type class
    template < class BASE_TYPE >
    class FactoryBuilderBase
    {
      private:
      public:
        virtual ~FactoryBuilderBase() {}

        virtual BASE_TYPE* create( Json::Value& ) const = 0;
    };


////////////////////////////////////////////////////////////////////////////////////////////////////
    // Derived implemented type wrappers


    template < class BASE, class DERIVED >
    class FactoryBuilder : public FactoryBuilderBase<BASE>
    {
      private:
      public:
        virtual ~FactoryBuilder() {}

        virtual BASE* create( Json::Value& ) const override;
    };


////////////////////////////////////////////////////////////////////////////////////////////////////
    // Template member function definitions

    template < class BASE, class DERIVED >
    BASE* FactoryBuilder::create( Json::Value& data ) const
    {
      MassProduceable* new_object = (MassProduceable*) new DERIVED();

      new_object->configure( data );

      return dynamic_cast<BASE*>( new_object );
    }

  }
}


#endif // REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_


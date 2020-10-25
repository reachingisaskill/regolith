
#ifndef REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_
#define REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Global/Json.h"
#include "Regolith/Architecture/MassProduceable.h"


namespace Regolith
{

  // Base Type class
  template < class BASE_TYPE, class ... ARGS >
  class FactoryBuilderBase
  {
    private:
    public:
      virtual ~FactoryBuilderBase() {}

      virtual BASE_TYPE* create( Json::Value&, ARGS... ) const = 0;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Derived implemented type wrappers


  template < class BASE, class DERIVED, class ... ARGS >
  class FactoryBuilder : public FactoryBuilderBase<BASE, ARGS...>
  {
    private:
    public:
      virtual ~FactoryBuilder() {}

      virtual BASE* create( Json::Value&, ARGS... ) const override;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Template member function definitions

  template < class BASE, class DERIVED, class ... ARGS >
  BASE* FactoryBuilder<BASE, DERIVED, ARGS...>::create( Json::Value& data, ARGS... args ) const
  {
    MassProduceable<ARGS...>* new_object = (MassProduceable<ARGS...>*) new DERIVED();

    new_object->configure( data, args... );

    BASE* base_pointer = dynamic_cast<BASE*>( new_object );

    if ( base_pointer == nullptr )
    {
      ERROR_STREAM << "FactoryBuilder : Failed to convert pointer to derived type!";
    }

    return base_pointer;
  }

}

#endif // REGOLITH_ARCHITECTURE_FACTORY_BUILDER_H_


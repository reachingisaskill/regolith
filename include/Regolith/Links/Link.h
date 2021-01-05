
#ifndef REGOLITH_LINKS_LINK_H_
#define REGOLITH_LINKS_LINK_H_

#include "Regolith/Global/Global.h"

#include <type_traits>


namespace Regolith
{

  template < class TARGET, class REQUESTER >
  class Link
  {
    struct FalseType : public std::false_type {};

    // This assert throws a compile time error, whenever the template parameters are expanded within this class.
    // Template specialisations are allowed.
    static_assert( FalseType::value, "A link is not permitted between the requested classes" );

    public :
      Link( TARGET* ) {}
  };

}

#endif // REGOLITH_LINKS_LINK_H_


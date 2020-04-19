
#include "Drawable.h"

#include <utility>


namespace Regolith
{

  Drawable::Drawable() :
    _theRenderer( nullptr )
  {
  }


  Drawable::Drawable( SDL_Renderer* rend ) :
    _theRenderer( rend )
  {
  }


  Drawable::Drawable( const Drawable&& d ) :
    _theRenderer( std::move( d._theRenderer ) )
  {
  }


  Drawable& Drawable::operator=( const Drawable&& d )
  {
    _theRenderer = std::move( d._theRenderer );

    return *this;
  }


  Drawable::~Drawable()
  {
  }

}


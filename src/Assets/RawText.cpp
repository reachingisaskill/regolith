
#include "Regolith/Assets/RawText.h"

namespace Regolith
{

  std::string* loadText( std::string filename )
  {
    std::ifstream reader( filename );

    std::string* data = new std::string();
    data->assign( std::istreambuf_iterator<char>( reader ), std::istreambuf_iterator<char>() );

    return data;
  }

}


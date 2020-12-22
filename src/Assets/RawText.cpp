
#include "Regolith/Assets/RawText.h"


namespace Regolith
{

  RawText loadRawText( TextDetail details )
  {
    RawText raw_text;
    raw_text.text = new std::string();

    std::ifstream reader( details.filename );
    std::stringstream buffer;

    // Load into the buffer
    buffer << reader.rdbuf();
    size_t size = buffer.str().size() - 1;

    // Reserve the right amount of space (skipping the last newline character)
    raw_text.text->reserve( size );

    // Load
    raw_text.text->assign( buffer.str(), 0, size );

    return raw_text;
  }

}


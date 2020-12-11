
#ifndef REGOLITH_GAMEPLAY_TEXT_H_
#define REGOLITH_GAMEPLAY_TEXT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  // Forward declarations
  class DataHandler;

  // Function that loads the text from the text file
  std::string* loadText( std::string );

////////////////////////////////////////////////////////////////////////////////O
  // Raw text struct

  struct RawText
  {
    std::string* text;

    RawText() : text( nullptr ) {}
  };


////////////////////////////////////////////////////////////////////////////////O
  // Text class definition

  class Text
  {
    private:
      RawText* _rawText;

      AlignmentMode _horizontalAlignment;
      AlignmentMode _verticalAlignment;

    public:
      // Empty con/destructor
      Text();
      ~Text();

      // Configure the font object
      void configure( Json::Value&, DataHandler& );

      // Return const reference to the actual string object
      const std::string* getRawText() const { return _rawText->text; }

  };

}

#endif // REGOLITH_GAMEPLAY_TEXT_H_


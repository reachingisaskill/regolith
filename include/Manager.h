
#ifndef __REGOLITH__MANAGER_H__
#define __REGOLITH__MANAGER_H__

#include "Singleton.h"
#include "Window.h"
#include "Scene.h"
#include "TextureBuilder.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <vector>
#include <map>
#include <string>


namespace Regolith
{
  // Useful typedefs
  typedef std::vector< Scene* > SceneList;
  typedef std::map< std::string, TTF_Font* > FontMap;


  // Manager class
  // Global storage for all scenes, renderers and windows.
  // Current version limited to a single window.
  class Manager : public Singleton< Manager >
  {
    friend class Singleton< Manager >; // Required to enable the singleton pattern

    private:
      Window* _theWindow;
      SDL_Renderer* _theRenderer;
      TextureBuilder* _theBuilder;
      SceneList _scenes;
      FontMap _fonts;

      std::string _title;
      TTF_Font* _defaultFont;
      SDL_Color _defaultColor;


    protected:
      Manager();

    public:
      virtual ~Manager();

      // Initialise the manager class from the configuration file
      void init( std::string );

      // Return a pointer to the texture builder
      TextureBuilder* getTextureBuilder() { return _theBuilder; }

      // Get the pointer to the window
      SDL_Renderer* getRendererPointer() { return _theRenderer; }

      // Get the pointer to the window
      Window* getWindowPointer() { return _theWindow; }

      // Return a pointer to the required font data
      TTF_Font* getFontPointer( std::string );

      // Return a pointer to the required scene
      Scene* getScene( size_t );

      // Return the number of scenes
      size_t getNumberScenes() { return _scenes.size(); }

      // Return the default font
      TTF_Font* getDefaultFont() { return _defaultFont; }

      // Return the default colour
      SDL_Color getDefaultColour() { return _defaultColor; }
  };

}

#endif // __REGOLITH__MANAGER_H__


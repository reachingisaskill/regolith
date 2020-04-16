
#include "regolith.h"

#include <SDL2/SDL.h>
#include <json/json.h>
#include "logtastic.h"


const int screen_width = 1024;
const int screen_height = 768;

int default_render_red = 80;
int default_render_green = 80;
int default_render_blue = 80;
int default_render_alpha = 255;

const char* test_config = "test_data/test_config.json";



using namespace Regolith;

int main( int, char** )
{
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );
  logtastic::init( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  try
  {
    Manager* man = Manager::createInstance();
    man->init( test_config );

    SDL_Renderer* theRenderer = man->getRendererPointer();
    Window* theWindow = man->getWindowPointer();

    bool quit = false;

    while ( ! quit )
    {
      SDL_Event e;
      while ( SDL_PollEvent( &e ) != 0 )
      {
        if ( e.type == SDL_QUIT )
          quit = true;

        theWindow->handleEvent( e );
      }

      SDL_SetRenderDrawColor( theRenderer, default_render_red, default_render_green, default_render_blue, default_render_alpha );
      SDL_RenderClear( theRenderer );

      




      SDL_RenderPresent( theRenderer );
    }


    Manager::killInstance();
  }
  catch ( Exception& ex )
  {
    FAILURE_LOG( ex.what() );
    std::cerr << ex.elucidate();
    return 0;
  }

  logtastic::stop();
  return 0;
}


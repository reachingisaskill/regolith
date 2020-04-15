
#include <SDL2/SDL.h>

#include "regolith.h"

#include "logtastic.h"


const int screen_width = 1024;
const int screen_height = 768;

int default_render_red = 80;
int default_render_green = 80;
int default_render_blue = 80;
int default_render_alpha = 255;

const char* ball_texture_path = "test_data/test_ball.png";
const char* background_texture_path = "test_data/test_background.png";

const char* window_title = "Regolith Integration Test";


void init();

using namespace Regolith;

int main( int, char** )
{
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );
  logtastic::init( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  try
  {
    init();

    Window theWindow( window_title );

    SDL_Renderer* theRenderer = theWindow.init( screen_width, screen_height );
    SDL_SetRenderDrawColor( theRenderer, 255, 255, 255, 255 );

    bool quit = false;

    while ( ! quit )
    {
      SDL_Event e;
      while ( SDL_PollEvent( &e ) != 0 )
      {
        if ( e.type == SDL_QUIT )
          quit = true;

        theWindow.handleEvent( e );
      }

      SDL_SetRenderDrawColor( theRenderer, default_render_red, default_render_green, default_render_blue, default_render_alpha );
      SDL_RenderClear( theRenderer );


      SDL_RenderPresent( theRenderer );
    }


    theWindow.free();
    SDL_DestroyRenderer( theRenderer );
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


void init()
{
  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    Exception ex( "init()", "Failed to initialise SDL" );
    ex.addDetail( "SDL Error", SDL_GetError() );
    throw ex;
  }

  if ( TTF_Init() == -1 )
  {
    Exception ex( "init()", "Failed to initialise TTF" );
    ex.addDetail( "TTF Error", TTF_GetError() );
    throw ex;
  }

  int imgFlags = IMG_INIT_PNG;
  if ( ! ( IMG_Init( imgFlags ) & IMG_INIT_PNG ) )
  {
    Exception ex( "init()", "Failed to initialise IMG" );
    ex.addDetail( "IMG Error", IMG_GetError() );
    throw ex;
  }
}


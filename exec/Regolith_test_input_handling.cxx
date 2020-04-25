//#define TESTASS_APPROX_LIMIT 1.0E-6

#include "regolith.h"
#include "TestDrawable.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>


using namespace Regolith;


int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_input_handling.log" );
  logtastic::start( "Regolith - Input Handling Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Input Handling" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

  InputHandler handler;

  handler.registerAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_SPACE, INPUT_ACTION_JUMP );

  handler.registerAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_W, INPUT_ACTION_MOVE_UP );
  handler.registerAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_S, INPUT_ACTION_MOVE_DOWN );
  handler.registerAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_A, INPUT_ACTION_MOVE_LEFT );
  handler.registerAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_D, INPUT_ACTION_MOVE_RIGHT );

  TestDrawable* testObject = new TestDrawable();

  testObject->registerEvents( &handler );

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Action Registration" );
  {
    ASSERT_EQUAL( handler.getRegisteredAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_SPACE ), INPUT_ACTION_JUMP );
    ASSERT_EQUAL( handler.getRegisteredAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_W ), INPUT_ACTION_MOVE_UP );
    ASSERT_EQUAL( handler.getRegisteredAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_S ), INPUT_ACTION_MOVE_DOWN );
    ASSERT_EQUAL( handler.getRegisteredAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_A ), INPUT_ACTION_MOVE_LEFT );
    ASSERT_EQUAL( handler.getRegisteredAction( INPUT_EVENT_KEYBOARD, SDL_SCANCODE_D ), INPUT_ACTION_MOVE_RIGHT );

    InputHandler::DrawableSet the_set = handler.getRegisteredObjects( INPUT_ACTION_JUMP );
    ASSERT_EQUAL( the_set.size(), (size_t)1 );
    ASSERT_FALSE( the_set.find( testObject ) == the_set.end() );

    the_set = handler.getRegisteredObjects( INPUT_ACTION_MOVE_UP );
    ASSERT_EQUAL( the_set.size(), (size_t)1 );
    ASSERT_FALSE( the_set.find( testObject ) == the_set.end() );

    the_set = handler.getRegisteredObjects( INPUT_ACTION_MOVE_DOWN );
    ASSERT_EQUAL( the_set.size(), (size_t)1 );
    ASSERT_FALSE( the_set.find( testObject ) == the_set.end() );

    the_set = handler.getRegisteredObjects( INPUT_ACTION_MOVE_LEFT );
    ASSERT_EQUAL( the_set.size(), (size_t)1 );
    ASSERT_FALSE( the_set.find( testObject ) == the_set.end() );

    the_set = handler.getRegisteredObjects( INPUT_ACTION_MOVE_RIGHT );
    ASSERT_EQUAL( the_set.size(), (size_t)1 );
    ASSERT_FALSE( the_set.find( testObject ) == the_set.end() );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Event Propagation" );
  {
    SDL_KeyboardEvent key_down;
    key_down.type = SDL_KEYDOWN;
    key_down.repeat = 0;
    key_down.keysym = { SDL_SCANCODE_SPACE, SDLK_SPACE, 0, 0 };

    SDL_KeyboardEvent key_up;
    key_up.type = SDL_KEYUP;
    key_up.repeat = 0;
    key_up.keysym = { SDL_SCANCODE_SPACE, SDLK_SPACE, 0, 0 };

    SDL_Event event;

    ASSERT_FALSE( testObject->getJump() );

    event.type = SDL_KEYDOWN;
    event.key = key_down;
    handler.handleEvent( event );

    ASSERT_TRUE( testObject->getJump() );

    event.type = SDL_KEYUP;
    event.key = key_up;
    handler.handleEvent( event );

    ASSERT_FALSE( testObject->getJump() );

    ASSERT_APPROX_EQUAL( testObject->getPosition().x(), 0.0 );

    SDL_KeyboardEvent key_left;
    key_left.type = SDL_KEYDOWN;
    key_left.repeat = 0;
    key_left.keysym = { SDL_SCANCODE_A, SDLK_a, 0, 0 };

    event.type = SDL_KEYDOWN;
    event.key = key_left;
    handler.handleEvent( event );

    ASSERT_APPROX_EQUAL( testObject->getVelocity().x(), -1.0 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  delete testObject;

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  logtastic::stop();
  return 0;
}


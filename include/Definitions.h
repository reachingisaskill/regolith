
#ifndef __REGOLITH__DEFINITIONS_H__
#define __REGOLITH__DEFINITIONS_H__


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#define REGOLITH_VERSION_NUMBER "0.1"


namespace Regolith
{

  const float pi = 3.14159265358979323846;

  const float e = 2.7182818284590452353602874;

  const float epsilon = 1.0E-6;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful Enumerations

  // Custom events added to SDL
  enum GameEvent
  {
    REGOLITH_QUIT,
    REGOLITH_SCENE_END,
    REGOLITH_SCENE_PAUSE,
    REGOLITH_FULLSCREEN,
    REGOLITH_EVENT_TOTAL
  };

//////////////////////////////////////////////////
  // Input possibilities

  enum InputAction
  {
    INPUT_ACTION_NULL,
    INPUT_MOVE,
    INPUT_MOVE_UP,
    INPUT_MOVE_UP_RIGHT,
    INPUT_MOVE_UP_LEFT,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_DOWN_RIGHT,
    INPUT_MOVE_DOWN_LEFT,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_AIM,
    INPUT_AIM_UP,
    INPUT_AIM_UP_RIGHT,
    INPUT_AIM_UP_LEFT,
    INPUT_AIM_DOWN,
    INPUT_AIM_DOWN_RIGHT,
    INPUT_AIM_DOWN_LEFT,
    INPUT_AIM_LEFT,
    INPUT_AIM_RIGHT,
    INPUT_LOOK,
    INPUT_LOOK_UP,
    INPUT_LOOK_UP_RIGHT,
    INPUT_LOOK_UP_LEFT,
    INPUT_LOOK_DOWN,
    INPUT_LOOK_DOWN_RIGHT,
    INPUT_LOOK_DOWN_LEFT,
    INPUT_LOOK_LEFT,
    INPUT_LOOK_RIGHT,
    INPUT_JUMP,
    INPUT_ATTACK,
    INPUT_PRIMARY_ATTACK,
    INPUT_SECONDARY_ATTACK,
    INPUT_DEFEND,
    INPUT_USE,
    INPUT_USE1,
    INPUT_USE2,
    INPUT_FIRE,
    INPUT_PRIMARY_FIRE,
    INPUT_SECONDARY_FIRE,
    INPUT_TERTIARY_FIRE,
    INPUT_RELOAD,
    INPUT_INTERACT,
    INPUT_INTERACT1,
    INPUT_INTERACT2,
    INPUT_MELEE,
    INPUT_MELEE1,
    INPUT_MELEE2,
    INPUT_CROUCH,
    INPUT_DUCK,
    INPUT_DASH,
    INPUT_PARRY,
    INPUT_DODGE,
    INPUT_BLOCK,
    INPUT_TAUNT,
    INPUT_CLICK,

    INPUT_ACTION_TOTAL
  };


//////////////////////////////////////////////////
  // Supported Event types

  enum InputEventType
  {
    INPUT_EVENT_NULL,
    INPUT_KEYBOARD,
    INPUT_BUTTON,
    INPUT_CONTROLLER_AXIS,
    INPUT_MOUSE_BUTTON,
    INPUT_MOUSE_MOVE,
    INPUT_MOUSE_WHEEL,
    INPUT_JOYSTICK_MOVE,
    INPUT_JOYHAT_MOVE,

    INPUT_WINDOW_EVENT,

    INPUT_EVENT_TOTAL
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // String Lookups

  const char* const InputActionStrings[] = 
  {
    "input_null",
    "input_move",
    "input_move_up",
    "input_move_up_right",
    "input_move_up_left",
    "input_move_down",
    "input_move_down_right",
    "input_move_down_left",
    "input_move_left",
    "input_move_right",
    "input_aim",
    "input_aim_up",
    "input_aim_up_right",
    "input_aim_up_left",
    "input_aim_down",
    "input_aim_down_right",
    "input_aim_down_left",
    "input_aim_left",
    "input_aim_right",
    "input_look",
    "input_look_up",
    "input_look_up_right",
    "input_look_up_left",
    "input_look_down",
    "input_look_down_right",
    "input_look_down_left",
    "input_look_left",
    "input_look_right",
    "input_jump",
    "input_attack",
    "input_primary_attack",
    "input_secondary_attack",
    "input_defend",
    "input_use",
    "input_use1",
    "input_use2",
    "input_fire",
    "input_primary_fire",
    "input_secondary_fire",
    "input_tertiary_fire",
    "input_reload",
    "input_interact",
    "input_interact1",
    "input_interact2",
    "input_melee",
    "input_melee1",
    "input_melee2",
    "input_crouch",
    "input_duck",
    "input_dash",
    "input_parry",
    "input_dodge",
    "input_block",
    "input_taunt",
    "input_click"
  };
}

#endif // __REGOLITH__DEFINITIONS_H__


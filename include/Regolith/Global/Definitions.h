
#ifndef REGOLITH_DEFINITIONS_H_
#define REGOLITH_DEFINITIONS_H_

#include "Regolith/Global/Json.h"

#include <list>
#include <set>
#include <map>
#include <limits>
#include <string>
#include <thread>
#include <mutex>


namespace Regolith
{

  const float pi = 3.14159265358979323846;

  const float e = 2.7182818284590452353602874;

  const float epsilon = 1.0E-6;

  const unsigned NUMBER_MOUSE_BUTTONS = 7;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful Typedefs
  class RawTexture;
  class Drawable;
  class Moveable;
  class Collidable;
  class Clickable;

  typedef unsigned int TeamID;
  typedef size_t IDNumber;

  typedef std::list< Drawable* > DrawableList;
  typedef std::list< Moveable* > MoveableList;
  typedef std::list< Collidable* > CollidableList;
  typedef std::list< Animated* > AnimatedList;
  typedef std::map< TeamID, CollidableList > TeamMap;
  typedef std::set< Clickable* > ClickableSet;
  typedef std::set< IDNumber > IDNumberSet;

  typedef std::map< std::string, RawTexture > RawTextureMap;
  typedef RawTextureMap::value_type* RawTexturePointer;
  typedef std::set< RawTexturePointer > RawTextureCache;



  typedef std::unique_lock< std::mutex > UniqueLock;
  typedef std::lock_guard< std::mutex > GuardLock;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful Enumerations

  enum CameraMode
  {
    CAMERA_FIXED,
    CAMERA_FLYING,
    CAMERA_FOLLOWING,
    CAMERA_CONSTANT_VELOCITY
  };


  enum AlignmentMode
  {
    ALIGNMENT_LOW_EDGE,
    ALIGNMENT_CENTRE,
    ALIGNMENT_HIGH_EDGE
  };


  enum MusicState
  {
    MUSIC_STATE_STOPPED,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED
  };


  enum DefaultTeams : unsigned char
  {
    DEFAULT_TEAM_HUD = 0,
    DEFAULT_TEAM_ENVIRONMENT = 1,
    DEFAULT_TEAM_NPC = 2,
    DEFAULT_TEAM_PLAYER = 3,
    DEFAULT_TEAM_ENEMIES = 4,
    DEFAULT_TEAM_MAX = 255
  };


  enum MouseButton : unsigned char
  {
    MOUSE_BUTTON_NULL = 0,
    MOUSE_BUTTON_LEFT = SDL_BUTTON_LMASK,
    MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MMASK,
    MOUSE_BUTTON_RIGHT = SDL_BUTTON_RMASK,
    MOUSE_BUTTON_X1 = SDL_BUTTON_X1MASK,
    MOUSE_BUTTON_X2 = SDL_BUTTON_X2MASK,
    MOUSE_BUTTON_Y1,
    MOUSE_BUTTON_Y2,
    MOUSE_BUTTON_TOTAL
  };


//////////////////////////////////////////////////
  // Event enumerations

  // In-scene triggers
  // Don't forget to update the string names!
  enum ContextEvent : int
  {
    CONTEXT_EVENT_NULL,

    CONTEXT_EVENT_QUIT,

    CONTEXT_EVENT_RESPAWN,
    CONTEXT_EVENT_DEATH,

    CONTEXT_EVENT_NEXT,
    CONTEXT_EVENT_PREV,
    CONTEXT_EVENT_SKIP,

    CONTEXT_EVENT_SELECT,

    CONTEXT_END, // Stops the current context
    CONTEXT_FINISHED, // Tells a context that it's daughter has stopped

    CONTEXT_EVENT_OPTIONS, // Eveything beyond this point can be considered a numerical value

    CONTEXT_EVENT_TOTAL = std::numeric_limits<int>::max() // Set to the maximum unsinged int
  };


  enum RegolithEvent
  {
    REGOLITH_EVENT_NULL,
    REGOLITH_EVENT_QUIT,
    REGOLITH_EVENT_ENGINE_PAUSE,
    REGOLITH_EVENT_ENGINE_RESUME,
    REGOLITH_EVENT_CONTEXT_END,
    REGOLITH_EVENT_CONTEXT_PAUSE,
    REGOLITH_EVENT_CONTEXT_RESUME,
    REGOLITH_EVENT_FULLSCREEN,
    REGOLITH_EVENT_WIN_CONDITION,
    REGOLITH_EVENT_LOSE_CONDITION,
    REGOLITH_EVENT_GAMEOVER,

    REGOLITH_EVENT_WINDOW,
    REGOLITH_EVENT_DISPLAY,
    REGOLITH_EVENT_JOYSTICK_HARDWARE,
    REGOLITH_EVENT_CONTROLLER_HARDWARE,
    REGOLITH_EVENT_AUDIO_HARDWARE,

    REGOLITH_EVENT_CAMERA_RESIZE,
    REGOLITH_EVENT_DATA_LOADED,

    REGOLITH_EVENT_TOTAL
  };


  // Don't forget to update the string names!
  enum InputAction
  {
    INPUT_ACTION_NULL,
    INPUT_ACTION_QUIT,
    INPUT_ACTION_PAUSE,

    INPUT_ACTION_CLICK,
    INPUT_ACTION_MOUSE_MOVE,

    INPUT_ACTION_MOVE,
    INPUT_ACTION_MOVE_UP,
    INPUT_ACTION_MOVE_UP_RIGHT,
    INPUT_ACTION_MOVE_UP_LEFT,
    INPUT_ACTION_MOVE_DOWN,
    INPUT_ACTION_MOVE_DOWN_RIGHT,
    INPUT_ACTION_MOVE_DOWN_LEFT,
    INPUT_ACTION_MOVE_LEFT,
    INPUT_ACTION_MOVE_RIGHT,
    INPUT_ACTION_MOVE_HORIZONTAL,
    INPUT_ACTION_MOVE_VERTICAL,

    INPUT_ACTION_AIM,
    INPUT_ACTION_AIM_UP,
    INPUT_ACTION_AIM_UP_RIGHT,
    INPUT_ACTION_AIM_UP_LEFT,
    INPUT_ACTION_AIM_DOWN,
    INPUT_ACTION_AIM_DOWN_RIGHT,
    INPUT_ACTION_AIM_DOWN_LEFT,
    INPUT_ACTION_AIM_LEFT,
    INPUT_ACTION_AIM_RIGHT,
    INPUT_ACTION_AIM_HORIZONTAL,
    INPUT_ACTION_AIM_VERTICAL,

    INPUT_ACTION_LOOK,
    INPUT_ACTION_LOOK_UP,
    INPUT_ACTION_LOOK_UP_RIGHT,
    INPUT_ACTION_LOOK_UP_LEFT,
    INPUT_ACTION_LOOK_DOWN,
    INPUT_ACTION_LOOK_DOWN_RIGHT,
    INPUT_ACTION_LOOK_DOWN_LEFT,
    INPUT_ACTION_LOOK_LEFT,
    INPUT_ACTION_LOOK_RIGHT,
    INPUT_ACTION_LOOK_HORIZONTAL,
    INPUT_ACTION_LOOK_VERTICAL,

    INPUT_ACTION_JUMP,
    INPUT_ACTION_ATTACK,
    INPUT_ACTION_PRIMARY_ATTACK,
    INPUT_ACTION_SECONDARY_ATTACK,
    INPUT_ACTION_DEFEND,
    INPUT_ACTION_USE,
    INPUT_ACTION_USE1,
    INPUT_ACTION_USE2,
    INPUT_ACTION_FIRE,
    INPUT_ACTION_PRIMARY_FIRE,
    INPUT_ACTION_SECONDARY_FIRE,
    INPUT_ACTION_TERTIARY_FIRE,
    INPUT_ACTION_RELOAD,
    INPUT_ACTION_INTERACT,
    INPUT_ACTION_INTERACT1,
    INPUT_ACTION_INTERACT2,
    INPUT_ACTION_MELEE,
    INPUT_ACTION_MELEE1,
    INPUT_ACTION_MELEE2,
    INPUT_ACTION_CROUCH,
    INPUT_ACTION_DUCK,
    INPUT_ACTION_DASH,
    INPUT_ACTION_PARRY,
    INPUT_ACTION_DODGE,
    INPUT_ACTION_BLOCK,
    INPUT_ACTION_TAUNT,

    INPUT_ACTION_NEXT,
    INPUT_ACTION_PREV,
    INPUT_ACTION_CONTINUE,
    INPUT_ACTION_BACK,
    INPUT_ACTION_RETURN,
    INPUT_ACTION_SELECT,
    INPUT_ACTION_CANCEL,

    INPUT_ACTION_TOTAL
  };


//////////////////////////////////////////////////
  // Supported Event types

  enum InputEventType
  {
    INPUT_TYPE_NULL,
    INPUT_TYPE_KEYBOARD,
    INPUT_TYPE_BUTTON,
    INPUT_TYPE_CONTROLLER_AXIS,
    INPUT_TYPE_MOUSE_BUTTON,
    INPUT_TYPE_MOUSE_MOVE,
    INPUT_TYPE_MOUSE_WHEEL,
    INPUT_TYPE_JOYSTICK_MOVE,
    INPUT_TYPE_JOYHAT_MOVE,
    INPUT_TYPE_HARDWARE_EVENT,

    INPUT_TYPE_TOTAL
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // String Lookups

  const char* const InputActionStrings[] = 
  {
    "null",
    "quit",
    "pause",

    "click",
    "mouse_move",

    "move",
    "move_up",
    "move_up_right",
    "move_up_left",
    "move_down",
    "move_down_right",
    "move_down_left",
    "move_left",
    "move_right",
    "move_horizontal",
    "move_vertical",

    "aim",
    "aim_up",
    "aim_up_right",
    "aim_up_left",
    "aim_down",
    "aim_down_right",
    "aim_down_left",
    "aim_left",
    "aim_right",
    "aim_horizontal",
    "aim_vertical",

    "look",
    "look_up",
    "look_up_right",
    "look_up_left",
    "look_down",
    "look_down_right",
    "look_down_left",
    "look_left",
    "look_right",
    "look_horizontal",
    "look_vertical",

    "jump",
    "attack",
    "primary_attack",
    "secondary_attack",
    "defend",
    "use",
    "use1",
    "use2",
    "fire",
    "primary_fire",
    "secondary_fire",
    "tertiary_fire",
    "reload",
    "interact",
    "interact1",
    "interact2",
    "melee",
    "melee1",
    "melee2",
    "crouch",
    "duck",
    "dash",
    "parry",
    "dodge",
    "block",
    "taunt",

    "next",
    "prev",
    "continue",
    "back",
    "return",
    "select",
    "cancel",
  };

  const std::map< std::string, int > ScancodeStrings = 
  {
    { "a", 4 },
    { "b", 5 },
    { "c", 6 },
    { "d", 7 },
    { "e", 8 },
    { "f", 9 },
    { "g", 10 },
    { "h", 11 },
    { "i", 12 },
    { "j", 13 },
    { "k", 14 },
    { "l", 15 },
    { "m", 16 },
    { "n", 17 },
    { "o", 18 },
    { "p", 19 },
    { "q", 20 },
    { "r", 21 },
    { "s", 22 },
    { "t", 23 },
    { "u", 24 },
    { "v", 25 },
    { "w", 26 },
    { "x", 27 },
    { "y", 28 },
    { "z", 29 },
    { "1", 30 },
    { "2", 31 },
    { "3", 32 },
    { "4", 33 },
    { "5", 34 },
    { "6", 35 },
    { "7", 36 },
    { "8", 37 },
    { "9", 38 },
    { "0", 39 },
    { "return", 40 },
    { "escape", 41 },
    { "backspace", 42 },
    { "tab", 43 },
    { "space", 44 },
    { "minus", 45 },
    { "equals", 46 },
    { "leftbracket", 47 },
    { "rightbracket", 48 },
    { "backslash", 49 },
    { "nonushash", 50 },
    { "semicolon", 51 },
    { "apostrophe", 52 },
    { "grave", 53 },
    { "comma", 54 },
    { "period", 55 },
    { "slash", 56 },
    { "capslock", 57 },
    { "f1", 58 },
    { "f2", 59 },
    { "f3", 60 },
    { "f4", 61 },
    { "f5", 62 },
    { "f6", 63 },
    { "f7", 64 },
    { "f8", 65 },
    { "f9", 66 },
    { "f10", 67 },
    { "f11", 68 },
    { "f12", 69 },
    { "printscreen", 70 },
    { "scrolllock", 71 },
    { "pause", 72 },
    { "insert", 73 },
    { "home", 74 },
    { "pageup", 75 },
    { "delete", 76 },
    { "end", 77 },
    { "pagedown", 78 },
    { "right", 79 },
    { "left", 80 },
    { "down", 81 },
    { "up", 82 },
    { "numlockclear", 83 },
    { "kp_divide", 84 },
    { "kp_multiply", 85 },
    { "kp_minus", 86 },
    { "kp_plus", 87 },
    { "kp_enter", 88 },
    { "kp_1", 89 },
    { "kp_2", 90 },
    { "kp_3", 91 },
    { "kp_4", 92 },
    { "kp_5", 93 },
    { "kp_6", 94 },
    { "kp_7", 95 },
    { "kp_8", 96 },
    { "kp_9", 97 },
    { "kp_0", 98 },
    { "kp_period", 99 },
    { "nonusbackslash", 100 },
    { "application", 101 },
    { "power", 102 },
    { "kp_equals", 103 },
    { "f13", 104 },
    { "f14", 105 },
    { "f15", 106 },
    { "f16", 107 },
    { "f17", 108 },
    { "f18", 109 },
    { "f19", 110 },
    { "f20", 111 },
    { "f21", 112 },
    { "f22", 113 },
    { "f23", 114 },
    { "f24", 115 },
    { "execute", 116 },
    { "help", 117 },
    { "menu", 118 },
    { "select", 119 },
    { "stop", 120 },
    { "again", 121 },
    { "undo", 122 },
    { "cut", 123 },
    { "copy", 124 },
    { "paste", 125 },
    { "find", 126 },
    { "mute", 127 },
    { "volumeup", 128 },
    { "volumedown", 129 },
    { "lockingcapslock", 130 },
    { "lockingnumlock", 131 },
    { "lockingscrolllock", 132 },
    { "kp_comma", 133 },
    { "kp_equalsas400", 134 },
    { "international1", 135 },
    { "international2", 136 },
    { "international3", 137 },
    { "international4", 138 },
    { "international5", 139 },
    { "international6", 140 },
    { "international7", 141 },
    { "international8", 142 },
    { "international9", 143 },
    { "lang1", 144 },
    { "lang2", 145 },
    { "lang3", 146 },
    { "lang4", 147 },
    { "lang5", 148 },
    { "lang6", 149 },
    { "lang7", 150 },
    { "lang8", 151 },
    { "lang9", 152 },
    { "alterase", 153 },
    { "sysreq", 154 },
    { "cancel", 155 },
    { "clear", 156 },
    { "prior", 157 },
    { "return2", 158 },
    { "separator", 159 },
    { "out", 160 },
    { "oper", 161 },
    { "clearagain", 162 },
    { "crsel", 163 },
    { "exsel", 164 },
    { "kp_00", 176 },
    { "kp_000", 177 },
    { "thousandsseparator", 178 },
    { "decimalseparator", 179 },
    { "currencyunit", 180 },
    { "currencysubunit", 181 },
    { "kp_leftparen", 182 },
    { "kp_rightparen", 183 },
    { "kp_leftbrace", 184 },
    { "kp_rightbrace", 185 },
    { "kp_tab", 186 },
    { "kp_backspace", 187 },
    { "kp_a", 188 },
    { "kp_b", 189 },
    { "kp_c", 190 },
    { "kp_d", 191 },
    { "kp_e", 192 },
    { "kp_f", 193 },
    { "kp_xor", 194 },
    { "kp_power", 195 },
    { "kp_percent", 196 },
    { "kp_less", 197 },
    { "kp_greater", 198 },
    { "kp_ampersand", 199 },
    { "kp_dblampersand", 200 },
    { "kp_verticalbar", 201 },
    { "kp_dblverticalbar", 202 },
    { "kp_colon", 203 },
    { "kp_hash", 204 },
    { "kp_space", 205 },
    { "kp_at", 206 },
    { "kp_exclam", 207 },
    { "kp_memstore", 208 },
    { "kp_memrecall", 209 },
    { "kp_memclear", 210 },
    { "kp_memadd", 211 },
    { "kp_memsubtract", 212 },
    { "kp_memmultiply", 213 },
    { "kp_memdivide", 214 },
    { "kp_plusminus", 215 },
    { "kp_clear", 216 },
    { "kp_clearentry", 217 },
    { "kp_binary", 218 },
    { "kp_octal", 219 },
    { "kp_decimal", 220 },
    { "kp_hexadecimal", 221 },
    { "lctrl", 224 },
    { "lshift", 225 },
    { "lalt", 226 },
    { "lgui", 227 },
    { "rctrl", 228 },
    { "rshift", 229 },
    { "ralt", 230 },
    { "rgui", 231 },
    { "mode", 257 },
    { "audionext", 258 },
    { "audioprev", 259 },
    { "audiostop", 260 },
    { "audioplay", 261 },
    { "audiomute", 262 },
    { "mediaselect", 263 },
    { "www", 264 },
    { "mail", 265 },
    { "calculator", 266 },
    { "computer", 267 },
    { "ac_search", 268 },
    { "ac_home", 269 },
    { "ac_back", 270 },
    { "ac_forward", 271 },
    { "ac_stop", 272 },
    { "ac_refresh", 273 },
    { "ac_bookmarks", 274 },
    { "brightnessdown", 275 },
    { "brightnessup", 276 },
    { "displayswitch", 277 },
    { "kbdillumtoggle", 278 },
    { "kbdillumdown", 279 },
    { "kbdillumup", 280 },
    { "eject", 281 },
    { "sleep", 282 },
    { "app1", 283 },
    { "app2", 284 },
    { "audiorewind", 285 },
    { "audiofastforward", 286 }
  };

  const char* const RegolithEventStrings[] =
  {
    "null",
    "quit",
    "engine_pause",
    "engine_resume",
    "context_end",
    "context_pause",
    "context_resume",
    "fullscreen",
    "win_condition",
    "lose_condition",
    "gameover",
    "window",
    "display",
    "joystick_hardware",
    "controller_hardware",
    "audio_hardware",
    "camera_resize"
  };


  // Context Event strings
  const char* const ContextEventStrings[] = 
  {
    "null",
    "quit",
    "respawn",
    "death",
    "next",
    "prev",
    "skip",
    "select",
    "end",
    "finished",
  };


  const char* const MouseButtonStrings[] =
  {
    "null",
    "left",
    "middle",
    "right",
    "x1",
    "x2",
    "y1",
    "y2"
  };


  const char* const ControllerButtonStrings[] =
  {
    "A",
    "B",
    "X",
    "Y",
    "back",
    "guide",
    "start",
    "left_stick",
    "right_stick",
    "left_shoulder",
    "right_shoulder",
    "dpad_up",
    "dpad_down",
    "dpad_left",
    "dpad_right",
    "null"
  };


  const char* const ControllerAxisStrings[] =
  {
    "leftx",
    "lefty",
    "rightx",
    "righty",
    "triggerleft",
    "triggerright",
    "null"
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Enumeration lookup functions
  InputAction getActionID( std::string );

  SDL_Scancode getScancodeID( std::string );

  RegolithEvent getRegolithEventID( std::string );

  ContextEvent getContextEventID( std::string );

  MouseButton getMouseButtonID( std::string );

  SDL_GameControllerButton getButtonID( std::string );

  SDL_GameControllerAxis getAxisID( std::string );
}

#endif // REGOLITH_DEFINITIONS_H_


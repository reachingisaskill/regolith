
#ifndef REGOLITH_GLOBAL_GLOBAL_H_
#define REGOLITH_GLOBAL_GLOBAL_H_


////////////////////////////////////////////////////////////////////////////////////////////////////
// Logtastic configuration
#define LOGTASTIC_FUNCTION_NAME ""
//#define LOGTASTIC_DEBUG_OFF
#include "logtastic.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global macro definitions

// Uncomment this to tell components that it will be running inside valgrind (more reliable - but slower - thread management).
#define REGOLITH_VALGRIND_BUILD 1


////////////////////////////////////////////////////////////////////////////////////////////////////
// Some bureaucracy headers
#include "Regolith/Global/Version.h"
#include "Regolith/Global/SDL.h"
#include "Regolith/Global/Json.h"
#include "Regolith/Global/Definitions.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Some standard utilities used everywhere
#include "Regolith/Utilities/Vector.h"
#include "Regolith/Utilities/Exception.h"


#endif // REGOLITH_GLOBAL_GLOBAL_H_


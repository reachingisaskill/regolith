
#ifndef REGOLITH_GLOBAL_GLOBAL_H_
#define REGOLITH_GLOBAL_GLOBAL_H_


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global macro definitions

#ifdef REGOLITH_DEBUG_VERSION

// This tells components that it will be running inside valgrind (more reliable - but slower - thread communication).
#define REGOLITH_VALGRIND_BUILD 1

#elif defined REGOLITH_RELEASE_VERSION

#define LOGTASTIC_DEBUG_OFF

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// Logtastic configuration
#define LOGTASTIC_FUNCTION_NAME ""
#include "logtastic.h"


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



#ifndef REGOLITH_GLOBAL_GLOBAL_H_
#define REGOLITH_GLOBAL_GLOBAL_H_


////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the version info first - affects the global defines
#include "Regolith/Global/Version.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global macro definitions

#if defined REGOLITH_VERSION_DEBUG

// This tells components that it will be running inside valgrind (more reliable - but slower - thread communication).
#define REGOLITH_VALGRIND_BUILD 1

#endif

#ifdef REGOLITH_VERSION_RELEASE

// Turn off debugging log statments
#define LOGTASTIC_DEBUG_OFF

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// Logtastic configuration
#ifdef LOGTASTIC_FUNCTION_NAME
#undef LOGTASTIC_FUNCTION_NAME
#endif
#define LOGTASTIC_FUNCTION_NAME ""
#include "logtastic.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Some bureaucracy headers
#include "Regolith/Global/SDL.h"
#include "Regolith/Global/Json.h"
#include "Regolith/Global/Definitions.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Some standard utilities used everywhere
#include "Regolith/Utilities/Vector.h"
#include "Regolith/Utilities/Exception.h"


#endif // REGOLITH_GLOBAL_GLOBAL_H_


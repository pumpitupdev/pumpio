#include "version.h"

#ifndef PIUIO_GITREV
#define PIUIO_GITREV "UNKNOWN"
#endif

#ifndef PIUIO_VERSION
#define PIUIO_VERSION "UNKNOWN"
#endif

const char* piuio_build_date = __DATE__ " " __TIME__;
const char* piuio_gitrev = PIUIO_GITREV;
const char* piuio_version = PIUIO_VERSION;
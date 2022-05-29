#include "version.h"

#ifndef PIUBTN_GITREV
#define PIUBTN_GITREV "UNKNOWN"
#endif

#ifndef PIUBTN_VERSION
#define PIUBTN_VERSION "UNKNOWN"
#endif

const char *piubtn_build_date = __DATE__ " " __TIME__;
const char *piubtn_gitrev = PIUBTN_GITREV;
const char *piubtn_version = PIUBTN_VERSION;
#ifndef PIUIO_VERSION_H
#define PIUIO_VERSION_H

/**
 * The build time and date of the library as defined by the build system.
 */
extern const char *piuio_build_date;

/**
 * The git revision hash the build is based on.
 */
extern const char *piuio_gitrev;

/**
 * Semantic version following MAJOR.MINOR.PATCH
 */
extern const char *piuio_version;

#endif
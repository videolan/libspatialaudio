/*############################################################################*/
/*#                                                                          #*/
/*#  Helpers to mark exported API symbols                                    #*/
/*#                                                                          #*/
/*#  Author(s):     Marvin Scholz                                            #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#ifndef SPATIALAUDIO_API_H
#define SPATIALAUDIO_API_H

#include "SpatialaudioConfig.h"

#if defined(__GNUC__) && (__GNUC__ >= 4)
// GCC/Clang symbol visibility
# define SPATIALAUDIO_EXPORT __attribute__((__visibility__("default")))
# define SPATIALAUDIO_IMPORT
#elif (defined(_WIN32) || defined(__CYGWIN__)) && \
      !defined(SPATIALAUDIO_STATIC)
// Windows symbol visibility, but only set while building
// or consuming the dynamic library.
# define SPATIALAUDIO_EXPORT __declspec(dllexport)
# define SPATIALAUDIO_IMPORT __declspec(dllimport)
#else
# define SPATIALAUDIO_EXPORT
# define SPATIALAUDIO_IMPORT
#endif

// While we are building the shared library, we need to build
// with the export attribute, else with the import attribute.
#if defined(SPATIALAUDIO_COMPILATION)
# define SPAUDIO_API SPATIALAUDIO_EXPORT
#else
# define SPAUDIO_API SPATIALAUDIO_IMPORT
#endif

// Deprecation annotation
// For GCC 12 and older we must not mix the attribute syntax
// styles, so in that case, we use __attribute__(()) here, else
// we just use the normal C++ attribute syntax
#if defined(__GNUC__) && (__GNUC__ < 13)
# define SPAUDIO_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
# define SPAUDIO_DEPRECATED(msg) [[deprecated(msg)]]
#endif

#endif /* SPATIALAUDIO_API_H */

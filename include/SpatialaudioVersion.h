/*############################################################################*/
/*#                                                                          #*/
/*#  libspatialaudio API version information                                 #*/
/*#                                                                          #*/
/*#  Author(s):     Marvin Scholz                                            #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#ifndef SPATIALAUDIO_VERSION_H
#define SPATIALAUDIO_VERSION_H

#define SPAUDIO_STRINGIFY(arg) SPAUDIO_STR(arg)
#define SPAUDIO_STR(arg) #arg
#define SPAUDIO_CONCACT_VERSION(maj, min, pat) \
    SPAUDIO_STRINGIFY(maj) "." SPAUDIO_STRINGIFY(min) "." SPAUDIO_STRINGIFY(pat)

/**
 * libspatialaudio major API version
 *
 * This is incremented whenever there are breaking
 * API/ABI changes. Building against a major version < N
 * but linking against major version N is not supported.
 */
#define SPATIALAUDIO_API_VERSION_MAJOR 2
/**
 * libspatialaudio minor API version
 */
#define SPATIALAUDIO_API_VERSION_MINOR 0
/**
 * libspatialaudio patch API version
 */
#define SPATIALAUDIO_API_VERSION_PATCH 0

#define SPATIALAUDIO_API_VERSION_STRING \
    SPAUDIO_CONCACT_VERSION(SPATIALAUDIO_API_VERSION_MAJOR, \
                            SPATIALAUDIO_API_VERSION_MINOR, \
                            SPATIALAUDIO_API_VERSION_PATCH)

#endif /* SPATIALAUDIO_VERSION_H */

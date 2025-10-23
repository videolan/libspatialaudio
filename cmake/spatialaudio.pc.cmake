prefix=@CMAKE_INSTALL_PREFIX@
libdir=${prefix}/@CMAKE_INSTALL_LIBDIR@
includedir=${prefix}/include

Name: spatialaudio
Description: Spatial audio rendering library
Version: @PROJECT_VERSION@
Libs: -L${libdir} -lspatialaudio @MYSOFA_LIB@ -lm -lz
Cflags: -I${includedir} @MYSOFA_INCLUDE@

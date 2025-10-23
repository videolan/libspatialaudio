cmake_minimum_required(VERSION 3.5)

# ABI version
# Do not forget to increase the major version when breaking ABI!
set(LIBRARY_SOVERSION_MAJOR "2")
set(LIBRARY_SOVERSION_MINOR "0")
set(LIBRARY_SOVERSION_PATCH "0")

project(spatialaudio VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)

include(GNUInstallDirs)

list(APPEND headers
    include/adm/Conversions.h
    include/adm/MappingRules.h
    include/adm/CartesianLoudspeakerLayouts.h
    include/adm/DirectSpeakerGainCalc.h
    include/RendererMetadata.h
    include/Renderer.h
    include/adm/AllocentricPannerGainCalc.h
    include/adm/AllocentricExtent.h
    include/AmbisonicAllRAD.h
    include/AmbisonicBase.h
    include/AmbisonicBinauralizer.h
    include/AmbisonicCommons.h
    include/AmbisonicDecoder.h
    include/AmbisonicDecoderPresets.h
    include/AmbisonicEncoderDist.h
    include/AmbisonicEncoder.h
    include/AmbisonicMicrophone.h
    include/AmbisonicProcessor.h
    include/AmbisonicPsychoacousticFilters.h
    include/AmbisonicRotator.h
    include/Ambisonics.h
    include/AmbisonicShelfFilters.h
    include/AmbisonicOptimFilters.h
    include/AmbisonicSource.h
    include/AmbisonicSpeaker.h
    include/AmbisonicZoomer.h
    include/BFormat.h
    include/Coordinates.h
    include/Decorrelator.h
    include/adm/GainCalculator.h
    include/GainInterp.h
    include/hrtf/hrtf.h
    include/hrtf/mit_hrtf.h
    include/hrtf/sofa_hrtf.h
    include/LoudspeakerLayouts.h
    include/LoudspeakerLayoutHulls.h
    include/mit_hrtf_filter.h
    include/mit_hrtf_lib.h
    include/normal/mit_hrtf_normal_44100.h
    include/normal/mit_hrtf_normal_48000.h
    include/normal/mit_hrtf_normal_88200.h
    include/normal/mit_hrtf_normal_96000.h
    include/PointSourcePannerGainCalc.h
    include/adm/PolarExtent.h
    include/RegionHandlers.h
    include/Screen.h
    include/ScreenCommon.h
    include/SpeakersBinauralizer.h
    include/Tools.h
    include/ObjectPanner.h
    include/t_design_5200.h
    source/kiss_fft/kiss_fft.h
    source/kiss_fft/kiss_fftr.h
    include/dsp/IIRFilter.h
    include/dsp/LinkwitzRileyIIR.h
)

list(APPEND sources
    source/AmbisonicEncoder.cpp
    source/AmbisonicMicrophone.cpp
    source/AmbisonicCommons.cpp
    source/mit_hrtf_lib.c
    source/AmbisonicProcessor.cpp
    source/AmbisonicRotator.cpp
    source/AmbisonicDecoder.cpp
    source/AmbisonicBinauralizer.cpp
    source/AmbisonicSource.cpp
    source/AmbisonicShelfFilters.cpp
    source/AmbisonicOptimFilters.cpp
    source/hrtf/mit_hrtf.cpp
    source/hrtf/sofa_hrtf.cpp
    source/BFormat.cpp
    source/SpeakersBinauralizer.cpp
    source/kiss_fft/kiss_fftr.c
    source/kiss_fft/kiss_fft.c
    source/AmbisonicAllRAD.cpp
    source/AmbisonicBase.cpp
    source/AmbisonicSpeaker.cpp
    source/AmbisonicEncoderDist.cpp
    source/AmbisonicZoomer.cpp
    source/Decorrelator.cpp
    source/adm/DirectSpeakerGainCalc.cpp
    source/Renderer.cpp
    source/adm/AllocentricPannerGainCalc.cpp
    source/adm/AllocentricExtent.cpp
    source/adm/GainCalculator.cpp
    source/GainInterp.cpp
    source/PointSourcePannerGainCalc.cpp
    source/adm/PolarExtent.cpp
    source/RegionHandlers.cpp
    source/Screen.cpp
    source/ObjectPanner.cpp
    source/dsp/IIRFilter.cpp
    source/dsp/LinkwitzRileyIIR.cpp
    source/LoudspeakerLayouts.cpp
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(MySofa QUIET)
set(HAVE_MYSOFA ${MYSOFA_FOUND})

include_directories(include include/adm include/dsp include/hrtf source source/kiss_fft ${PROJECT_BINARY_DIR})

if(MYSOFA_FOUND)
    include_directories(${MYSOFA_INCLUDE_DIRS})
endif(MYSOFA_FOUND)

if(BUILD_SHARED_LIBS)
    add_library(spatialaudio-shared SHARED ${sources})
    if(MYSOFA_FOUND)
        target_link_libraries(spatialaudio-shared ${MYSOFA_LIBRARIES})
    endif(MYSOFA_FOUND)
    SET_TARGET_PROPERTIES(spatialaudio-shared PROPERTIES OUTPUT_NAME spatialaudio CLEAN_DIRECT_OUTPUT 1)
    set_property(TARGET spatialaudio-shared PROPERTY VERSION "${LIBRARY_SOVERSION_MAJOR}.${LIBRARY_SOVERSION_MINOR}.${LIBRARY_SOVERSION_PATCH}")
    set_property(TARGET spatialaudio-shared PROPERTY SOVERSION ${LIBRARY_SOVERSION_MAJOR} )
    set_property(TARGET spatialaudio-shared PROPERTY C_VISIBILITY_PRESET hidden)
    install(TARGETS spatialaudio-shared LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
else()
    add_library(spatialaudio-static STATIC ${sources})
    if(MYSOFA_FOUND)
        target_link_libraries(spatialaudio-static ${MYSOFA_LIBRARIES})
    endif(MYSOFA_FOUND)
    SET_TARGET_PROPERTIES(spatialaudio-static PROPERTIES OUTPUT_NAME spatialaudio CLEAN_DIRECT_OUTPUT 1 POSITION_INDEPENDENT_CODE ON)
    install(TARGETS spatialaudio-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_SHARED_LIBS)

option(HAVE_MIT_HRTF "Should MIT HRTF be built-in" ON)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

if(MYSOFA_FOUND)
    message("Found mysofa!")
    set(MYSOFA_LIB "-L${MYSOFA_LIBRARY_DIRS} -lmysofa")
    set(MYSOFA_INCLUDE "-I${MYSOFA_INCLUDE_DIRS}")
endif(MYSOFA_FOUND)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/spatialaudio.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    @ONLY
)

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/spatialaudio)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/spatialaudio)

enable_testing()
add_subdirectory(tests)

#Tarballs generation
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_GENERATOR "TBZ2")
list(APPEND CPACK_SOURCE_IGNORE_FILES
    "/build.*/"
    "/\\\\..+"
)
include(CPack)

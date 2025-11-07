cmake_minimum_required(VERSION 3.23)

# ABI version
# Do not forget to increase the major version when breaking ABI!
set(LIBRARY_SOVERSION_MAJOR "2")
set(LIBRARY_SOVERSION_MINOR "0")
set(LIBRARY_SOVERSION_PATCH "0")

project(spatialaudio VERSION 0.4.0)

# Options
option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)
option(HAVE_MIT_HRTF "Should MIT HRTF be built-in" ON)

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Dependencies
find_package(MySofa QUIET)
set(HAVE_MYSOFA ${MYSOFA_FOUND})

# Spatialaudio library
add_library(spatialaudio)
add_library(Spatialaudio::spatialaudio ALIAS spatialaudio)
set_target_properties(spatialaudio
    PROPERTIES
        OUTPUT_NAME spatialaudio
        POSITION_INDEPENDENT_CODE ON
        VERSION "${LIBRARY_SOVERSION_MAJOR}.${LIBRARY_SOVERSION_MINOR}.${LIBRARY_SOVERSION_PATCH}"
        SOVERSION ${LIBRARY_SOVERSION_MAJOR}
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
)
target_compile_features(spatialaudio PUBLIC cxx_std_14)
target_sources(spatialaudio
    PRIVATE
        source/AmbisonicEncoder.cpp
        source/AmbisonicMicrophone.cpp
        source/AmbisonicCommons.cpp
        source/mit_hrtf/mit_hrtf_lib.c
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

    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS
            include
            source
        FILES
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
            include/PointSourcePannerGainCalc.h
            include/adm/PolarExtent.h
            include/RegionHandlers.h
            include/Screen.h
            include/ScreenCommon.h
            include/SpeakersBinauralizer.h
            include/Tools.h
            include/ObjectPanner.h
            include/kiss_fft/kiss_fft.h
            include/kiss_fft/kiss_fftr.h
            include/dsp/IIRFilter.h
            include/dsp/LinkwitzRileyIIR.h
            include/SpatialaudioAPI.h
)

target_include_directories(spatialaudio
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(spatialaudio
    PRIVATE
        SPATIALAUDIO_COMPILATION
)

if(MYSOFA_FOUND)
    message("Found mysofa!")
    set(MYSOFA_LIB "-L${MYSOFA_LIBRARY_DIRS} -lmysofa")
    set(MYSOFA_INCLUDE "-I${MYSOFA_INCLUDE_DIRS}")
    target_include_directories(spatialaudio
        PUBLIC ${MYSOFA_INCLUDE_DIRS})
    target_link_libraries(spatialaudio ${MYSOFA_LIBRARIES})
endif(MYSOFA_FOUND)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/spatialaudio.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    @ONLY
)

install(TARGETS spatialaudio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/spatialaudio
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/spatialaudio)

enable_testing()
add_subdirectory(tests)

# Tarballs generation
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_GENERATOR "TBZ2")
list(APPEND CPACK_SOURCE_IGNORE_FILES
    "/build.*/"
    "/\\\\..+"
)
include(CPack)

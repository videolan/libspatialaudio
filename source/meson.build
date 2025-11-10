# Library files
spatialaudio_sources = files(
    'AmbisonicEncoder.cpp',
    'AmbisonicMicrophone.cpp',
    'AmbisonicCommons.cpp',
    'mit_hrtf/mit_hrtf_lib.c',
    'AmbisonicProcessor.cpp',
    'AmbisonicRotator.cpp',
    'AmbisonicDecoder.cpp',
    'AmbisonicBinauralizer.cpp',
    'AmbisonicSource.cpp',
    'AmbisonicShelfFilters.cpp',
    'AmbisonicOptimFilters.cpp',
    'hrtf/mit_hrtf.cpp',
    'hrtf/sofa_hrtf.cpp',
    'BFormat.cpp',
    'SpeakersBinauralizer.cpp',
    'kiss_fft/kiss_fftr.c',
    'kiss_fft/kiss_fft.c',
    'AmbisonicAllRAD.cpp',
    'AmbisonicBase.cpp',
    'AmbisonicSpeaker.cpp',
    'AmbisonicEncoderDist.cpp',
    'AmbisonicZoomer.cpp',
    'adm/DirectSpeakersGainCalc.cpp',
    'Renderer.cpp',
    'adm/AllocentricPannerGainCalc.cpp',
    'adm/AllocentricExtent.cpp',
    'Decorrelator.cpp',
    'adm/GainCalculator.cpp',
    'GainInterp.cpp',
    'PointSourcePannerGainCalc.cpp',
    'adm/PolarExtent.cpp',
    'RegionHandlers.cpp',
    'Screen.cpp',
    'dsp/IIRFilter.cpp',
    'dsp/LinkwitzRileyIIR.cpp',
    'LoudspeakerLayouts.cpp',
    'ObjectPanner.cpp',
)

spatialaudio_clike_args = ['-DSPATIALAUDIO_COMPILATION']

# Extract version info from header
spatialaudio_api_version = cpp.get_define('SPATIALAUDIO_API_VERSION_STRING',
    prefix : '#include "SpatialaudioVersion.h"',
    include_directories : spatialaudio_incdirs).strip().strip('"')

spatialaudio_lib = library(
    'spatialaudio',
    spatialaudio_sources,
    install : true,
    dependencies : dependencies,
    include_directories : spatialaudio_incdirs,
    version : spatialaudio_api_version,
    gnu_symbol_visibility : 'hidden',
    c_args : spatialaudio_clike_args,
    cpp_args : spatialaudio_clike_args,
)

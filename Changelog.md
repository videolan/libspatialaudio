## Changes in 0.4.0

- This is an API and ABI breaking release.
- Move Classes to the `spaudio` namespace.
- Simplified class names:
  - `CAdmDirectSpeakersGainCalc` to `AdmDirectSpeakersGainCalc`
  - `CAdmRenderer` to `Renderer`
  - `CAllocentricExtent` to `AllocentricExtent`
  - `CAllocentricPannerGainCalc` to `AllocentricPannerGainCalc`
  - `CAmbisonicAllRAD` to `AmbisonicAllRAD`
  - `CAmbisonicBase` to `AmbisonicBase`
  - `CAmbisonicBinauralizer` to `AmbisonicBinauralizer`
  - `CAmbisonicDecoder` to `AmbisonicDecoder`
  - `CAmbisonicEncoder` to `AmbisonicEncoder`
  - `CAmbisonicEncoderDist` to `AmbisonicEncoderDist`
  - `CAmbisonicMicrophone` to `AmbisonicMicrophone`
  - `CAmbisonicOptimFilters` to `AmbisonicOptimFilters`
  - `CAmbisonicPolarExtentHandler` to `AmbisonicPolarExtentHandler`
  - `CAmbisonicProcessor` to `AmbisonicProcessor`
  - `CAmbisonicRotator` to `AmbisonicRotator`
  - `CAmbisonicShelfFilters` to `AmbisonicShelfFilters`
  - `CAmbisonicSource` to `AmbisonicSource`
  - `CAmbisonicSpeaker` to `AmbisonicSpeaker`
  - `CAmbisonicSpreadPanner` to `AmbisonicSpreadPanner`
  - `CAmbisonicZoomer` to `AmbisonicZoomer`
  - `CBFormat` to `BFormat`
  - `CDecorrelator` to `Decorrelator`
  - `CGainCalculator` to `GainCalculator`
  - `CGainInterp` to `GainInterp`
  - `CIIRFilter` to `IIRFilter`
  - `CLinkwitzRileyIIR` to `LinkwitzRileyIIR`
  - `CPointSourcePannerGainCalc` to `PointSourcePannerGainCalc`
  - `CPolarExtentHandler` to `PolarExtentHandler`
  - `CPolarExtentHandlerBase` to `PolarExtentHandlerBase`
  - `CScreenEdgeLock` to `ScreenEdgeLockHandler`
  - `CScreenScaleHandler` to `ScreenScaleHandler`
  - `CSpreadPanner` to `SpreadPanner`
  - `CSpreadPannerBase` to `SpreadPannerBase`
- Changed LFE handling for the Renderer (formerly AdmRenderer) when rendering
  to binaural. Before, LFE channels would be discarded when rendering to
  binaural, following the ADM specification. Now they are included and
  a new argument `useLfeBinaural` was added to `Configure`, to change this
  behaviour if necessary.
- `OutputLayout` moved to the `LoutspeakerLayout.h` header.
- Removed/replaced some OutputLayout entries:
  - Replaced `OutputLayout::ITU_0_2_0`, use `OutputLayout::Stereo`
  - Replaced `OutputLayout::ITU_0_5_0`, use `OutputLayout::FivePointOne`
  - Replaced `OutputLayout::ITU_2_5_0`, use `OutputLayout::FivePointOnePointTwo`
  - Replaced `OutputLayout::ITU_4_5_0`, use `OutputLayout::FivePointOnePointFour`
  - Replaced `OutputLayout::ITU_4_5_1`, use `OutputLayout::FivePointOnePointFourPlusLow`
  - Replaced `OutputLayout::ITU_3_7_0`, use `OutputLayout::SevenPointOnePointThree`
  - Replaced `OutputLayout::ITU_3_7_0`, use `OutputLayout::SevenPointOnePointThree`
  - Replaced `OutputLayout::ITU_4_9_0`, use `OutputLayout::ThirteenPointOne`
  - Replaced `OutputLayout::ITU_9_10_3`, use `OutputLayout::TwentyTwoPointTwo`
  - Replaced `OutputLayout::ITU_0_7_0`, use `OutputLayout::SevenPointOne`
  - Replaced `OutputLayout::ITU_4_7_0`, use `OutputLayout::SevenPointOnePointFour`
  - Replaced `OutputLayout::_3p1p2`, use `OutputLayout::ThreePointOnePointTwo`
  - Replaced `OutputLayout::_2_7_0`, use `OutputLayout::SevenPointOnePointTwo`
  - Removed `OutputLayout::FivePointZero`
  - Removed `OutputLayout::SevenPointZero`
- CMake: Removed the `BUILD_STATIC_LIBS` option. Use the `BUILD_SHARED_LIBS`
  option to control if share or static libraries should be built. If both are
  needed, build twice explicitly.
- The `config.h` header file was removed in favor of `SpatialaudioConfig.h` with
  properly prefixed defines to prevent name clashes.

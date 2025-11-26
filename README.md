# Spatial audio encoding / decoding and binauralization library

**libspatialaudio** is an open-source, real-time and cross-platform C++ library for spatial audio encoding, decoding and rendering.
It provides a unified framework for working with different spatial audio formats and playback environments, from multichannel loudspeakers to headphones.

At its core is the `Renderer` class, which brings together multiple spatial audio techniques under one umbrella. Whether you are working with Ambisonics (HOA), audio objects, direct speaker feeds (e.g. 5.1/7.1), or binaural rendering, the `Renderer` provides a consistent interface for rendering multiple different streams.

Originally forked from **ambisonic-lib** by Aristotel Digenis it was expanded to support Higher Order Ambisonics *HOA* (**ACN/SN3D**) Ambisonics audio streams following the **Google spatial audio** specification:
<https://github.com/google/spatial-media/blob/master/docs/spatial-audio-rfc.md> and the **IETF codec Ambisonics** specification <https://tools.ietf.org/html/draft-ietf-codec-ambisonics>

It has since been expanded to include Object and speaker rendering, making it a more general purpose spatial audio library. These additions allow it to be used to render ITU Audio Definition Model **ADM** feeds (based on Rec. ITU-R BS.2127-1) and Alliance for Open Media's Immersive Audio and Formats **IAMF**.

## What You Can Do with libspatialaudio

**libspatialaudio** contains classes for implementing different spatial audio techniques and processing:

- **Ambisonics**: Encode, rotate, zoom, and decode HOA (up to 3rd order). Includes psychoacoustic optimizations for improved spatial perception. For more details on the Ambisonics capabilities of **libspatialaudio** please go [here](docs/AmbisonicsOverview.md).
- **Object-based Audio**: Encode and position individual sound sources in 3D space, rendered to speakers or headphones. Read more on how to spatialise object signals [here](docs/ObjectPanning.md).
- **Speaker Rendering**: Decode and route speaker streams or layouts to chosen output loudspeaker layouts. For a guide on how to render speaker signals see [here](docs/RendererOverview.md).
- **Binaural Rendering**: Render immersive audio for headphones using HRTFs (either custom SOFA files or the included MIT HRTF). Works with Ambisonics, object-based audio, or multichannel speaker layouts (5.1, 7.1, etc.). Read more on how to binauralise loudspeaker signals [here](docs/LoudspeakerBinauralization.md).

## Applications

While **libspatialaudio** is designed around core processing types rather than specific standards, it can be applied in workflows that use formats such as:

- **ADM (Audio Definition Model)**: for metadata-driven rendering of objects, speakers and HOA.
- **IAMF (Immersive Audio Model and Format)**: for next-generation immersive audio delivery.

Both of these application use the `Renderer` class to interpret metadata and route signals appropriately, whether for loudspeaker layouts or binaural output.

## Features

- Unified `Renderer` class for HOA, Object, Speaker, and Binaural processing.
- Ambisonics encoding, processing and decoding up to 3rd order (ACN/SN3D).
- Loudspeaker rendering to ITU and custom layouts.
- Binauralization with SOFA-based or built-in HRTFs.
- Object-based rendering with dynamic positioning.
- Suitable for both real-time and offline rendering workflows.
- Cross-platform C++ implementation.

## Learning About libspatialaudio

The documentation includes:

- background theory (mathematical, psychoacoustic, DSP)
  - All background theory sections have been written such that it should be accessible to anyone with some mathematical background.
However, even without a mathematical background you should still be able to follow what is going on.
- implementation details (lays out the choices made while writing the library).
- code examples (overviews of the main classes and code examples).

To learn more about the Ambisonics processors start [here](docs/AmbisonicsOverview.md).

To learn more about the main `Renderer` start [here](docs/AdmRendererOverview.md).

These two documents provide full examples of both signal flows, as well as links to more details for those who want to learn more or get more specific details.

For those who do not need to know the background theory, most documentation pages contain a link at the top to take you directly to details of the code, as well as an example of how to implement it.

## Building libspatialaudio

Clone the repository (if you haven't already):

```bash
git clone https://github.com/videolabs/libspatialaudio.git
cd libspatialaudio
```

To build **libspatialaudio**, either Meson or CMake can be used:

### Meson

1. Create a `build` directory and configure the project:

   ```bash
   meson setup build
   ```

2. Build the project:

   ```bash
   meson compile -C build
   ```

### CMake

1. Create a `build` directory and configure the project:

   ```bash
   cmake -B build
   ```

2. Build the project:

   ```bash
   cmake --build build
   ```

## License

libspatialaudio is released under LGPLv2.1 (or later) and is also available
under a commercial license. For full details see [here](LICENSE).

/*############################################################################*/
/*#                                                                          #*/
/*#  A renderer for Object, HOA and DirectSpeaker streams.                   #*/
/*#  Renderer                                                                #*/
/*#  Copyright © 2020 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      Renderer.h                                               #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#ifndef SPATIALAUDIO_RENDERER_H
#define SPATIALAUDIO_RENDERER_H

#include <vector>
#include <string>
#include "SpatialaudioAPI.h"
#include "Ambisonics.h"
#include "RendererMetadata.h"
#include "LoudspeakerLayouts.h"
#include "Tools.h"
#include "adm/Conversions.h"
#include "GainInterp.h"
#include "adm/DirectSpeakersGainCalc.h"
#include "Decorrelator.h"
#include "adm/GainCalculator.h"

namespace spaudio {

    /** This is a renderer for ADM streams. It aims to provide a simple way to
     *	add spatial data to the stream audio using the methods available in
     *	libspatialaudio.
     *
     *	CURRENT FUNCTIONALITY:
     *	- Spatialise Objects with a VBAP-type panner when output to speakers
     *	- Add HOA signal to the render to be decoded
     *	- Add DirectSpeaker tracks to be rendered
     *	- Set the output format to stereo, binaural, quad, and ITU layouts
     *	- Apply decorrelation to Objects and apply compensation delay to the direct signal
     *	- Handles exclusion zones, divergence, channel lock
     *	- Handles extent panning for both loudspeaker and binaural output
     *	- Handle screen scaling and screen edge lock
     *
     *	Required to meet full compliance with Rec. ITU-R BS.2127-0:
     *	- Handle Matrix types (need samples to be able to test)
     */
    class SPAUDIO_API Renderer
    {
    public:
        Renderer();
        ~Renderer();

        /** Configure the ADM Renderer.
         *
         * @param outputTarget			The target output layout.
         * @param hoaOrder				The ambisonic order of the signal to be rendered.
         * @param nSampleRate			Sample rate of the streams to be processed.
         * @param nSamples				The maximum number of samples in an audio frame to be added to the stream.
         * @param channelInfo			Information about the expected stream formats.
         * @param HRTFPath				Path to an HRTF to be used when the output layout is binaural.
         * @param useLfeBinaural		Flag if the LFE should be rendered or discarded when the output layout is binaural.
         * @param reproductionScreen	Screen details used for screen scaling/locking.
         * @param layoutPositions		(Optional) Real polar positions for each of the loudspeaker in the layout.
         * @return						Returns true if the class is correctly configured and ready to use.
         */
        bool Configure(OutputLayout outputTarget, unsigned int hoaOrder, unsigned int nSampleRate, unsigned int nSamples,const StreamInformation& channelInfo,
            std::string HRTFPath = "", bool useLfeBinaural = true, Optional<Screen> reproductionScreen = Optional<Screen>(), const std::vector<PolarPosition<double>>& layoutPositions = std::vector<PolarPosition<double>>());

        /** Add an audio Object to be rendered.
         *
         * @param pIn		Pointer to the object buffer to be rendered.
         * @param nSamples	Number of samples in the stream.
         * @param metadata	Metadata for the object stream.
         * @param nOffset	Number of samples of delay to applied to the signal.
         */
        void AddObject(float* pIn, unsigned int nSamples, const ObjectMetadata& metadata, unsigned int nOffset = 0);

        /** Adds an HOA stream to be rendered. Currently only supports SN3D normalisation.
         *
         * @param pHoaIn	The HOA audio channels to be rendered of size nAmbiCh x nSamples
         * @param nSamples	Number of samples in the stream.
         * @param metadata	Metadata for the HOA stream
         * @param nOffset	Number of samples of delay to applied to the signal.
         */
        void AddHoa(float** pHoaIn, unsigned int nSamples, const HoaMetadata& metadata, unsigned int nOffset = 0);

        /** Adds an DirectSpeaker stream to be rendered.
         *
         * @param pDirSpkIn	Pointer to the direct speaker stream.
         * @param nSamples	Number of samples in the stream.
         * @param metadata	Metadata for the DirectSpeaker stream
         * @param nOffset	Number of samples of delay to applied to the signal.
         */
        void AddDirectSpeaker(float* pDirSpkIn, unsigned int nSamples, const DirectSpeakerMetadata& metadata, unsigned int nOffset = 0);

        /** Adds a binaural signal to the output. If the output type was not set to Binaural at startup
         *	then the input is discarded.
         * @param pBinIn	The binaural stream of size 2 x nSamples
         * @param nSamples	Number of samples in the stream.
         * @param nOffset	Number of samples of delay to applied to the signal.
         */
        void AddBinaural(float** pBinIn, unsigned int nSamples, unsigned int nOffset = 0);

        /** Get the rendered audio. This should only be called after all streams have been added.
         * @param pRender	Rendered audio output.
         * @param nSamples	The number of samples to get
         */
        void GetRenderedAudio(float** pRender, unsigned int nSamples);

        /** Reset the processor. */
        void Reset();

        /** Get the number of speakers in the layout specified to Configure.
         * @return Number of output channels
         */
        unsigned int GetSpeakerCount();

        /** Set head orientation to apply head tracking when rendering to binaural.
         *  Rotations are applied in the order yaw-pitch-roll
         * @param orientation	Head orientation
         */
        void SetHeadOrientation(const RotationOrientation& orientation);

        /** Set the linear gain to be applied to the signal obtained from GetRenderedAudio().
         * @param outGain Linear gain to be applied to the rendered audio.
         */
        void SetOutputGain(double outGain);

    private:
        OutputLayout m_RenderLayout;
        // Number of channels in the array (use virtual speakers for binaural rendering)
        unsigned int m_nChannelsToRender = 2;
        // Number of channels in the output
        unsigned int m_nChannelsToOutput = 2;
        // Ambisonic order to be used for playback
        unsigned int m_HoaOrder = 3;
        // Number of ambisonic channels corresponding to the HOA order
        unsigned int m_nAmbiChannels = 16;
        // Maximum number of samples expected in a frame
        unsigned int m_nSamples;
        // Flag if LFE channels are to be rendered in binaural. The BEAR spec discards it (see EBU Tech 3396 Sec. 3.7.1)
        bool m_useLfeBinaural = true;

        StreamInformation m_channelInformation;

        Layout m_outputLayout;

        // Vector holding the last unique set metadata for each object in the stream
        std::vector<ObjectMetadata> m_objectMetadata;
        // A map from the channel index to the object index in the order the objects were listed
        // in the stream at configuration
        std::map<int, int> m_channelToObjMap;

        // Object metadata for internal use when converting to polar coordinates
        ObjectMetadata m_objMetaDataTmp;

        // Temp DirectSpeaker metadata when in binaural mode to ensure only the desired gain calculation elements are used
        DirectSpeakerMetadata m_dirSpkBinMetaDataTmp;

        // The channel indices of the tracks that can use a point source panner
        std::vector<std::pair<unsigned int, TypeDefinition>> m_pannerTrackInd;
        // Gain interpolators
        std::vector<GainInterp<double>> m_gainInterpDirect;
        std::vector<GainInterp<double>> m_gainInterpDiffuse;
        // The gain calculator for Object type channels
        std::unique_ptr<adm::ObjectGainCalculator> m_objectGainCalc;
        // The gain calculator for the DirectSpeaker channels
        std::unique_ptr<adm::DirectSpeakersGainCalc> m_directSpeakerGainCalc;
        // Gain interpolators for DirectSpeaker streams
        std::vector<GainInterp<double>> m_directSpeakerGainInterp;
        // Time in samples to interpolate from one metadata or output gain to the next
        unsigned int m_gainInterpTime = 0;

        // A map from the channel index to the DirectSpeaker index in the order the DirectSpeakers were listed
        // in the stream at configuration
        std::map<int, int> m_channelToDirectSpeakerMap;

        // Gain interpolators for HOA stream
        std::vector<GainInterp<double>> m_hoaGainInterp;
        // Ambisonic Decoder
        AmbisonicAllRAD m_hoaDecoder;
        // Ambisonic encoders to use convert from speaker feeds to HOA for binaural decoding
        std::vector<AmbisonicEncoder> m_hoaEncoders;
        // Ambisonic rotation for binaural with head-tracking
        AmbisonicRotator m_hoaRotate;
        // Ambisonic binaural decoder
        AmbisonicBinauralizer m_hoaBinaural;
        // Buffers to hold the HOA audio
        BFormat m_hoaAudioOut;
        // Buffers holding the output signal
        float** m_speakerOut = nullptr;
        // Buffers to hold the direct object audio
        float** m_speakerOutDirect = nullptr;
        // Buffers to hold the diffuse object audio
        float** m_speakerOutDiffuse = nullptr;
        // Buffers to the hold the virtual speaker layout signals when rendering to binaural
        float** m_virtualSpeakerOut = nullptr;
        // Buffers to hold binaural signals added via AddBinaural() when rendering to binaural
        float** m_binauralOut = nullptr;
        void ClearOutputBuffer();
        void ClearObjectDirectBuffer();
        void ClearObjectDiffuseBuffer();
        void ClearBinauralBuffer();
        void ClearVirtualSpeakerBuffer();

        // Decorrelator filter processor
        Decorrelator m_decorrelate;

        // Output gain
        double m_outGain = 1.0;
        std::vector<GainInterp<double>> m_outGainInterp;

        // A buffer containing all zeros to use to clear the HOA data
        std::unique_ptr<float[]> m_pZeros;
        void ClearHoaBuffer();

        // Temp vectors
        std::vector<double> m_directGains;
        std::vector<double> m_diffuseGains;
        std::vector<double> m_directSpeakerGains;

        /** Find the element of a vector matching the input. If the track types do not match or no matching elements then returns -1 */
        int GetMatchingIndex(const std::vector<std::pair<unsigned int, TypeDefinition>>& vector, unsigned int nElement, TypeDefinition trackType);

        /** Allocate internal 2D buffers of the specified size (nCh x nSamples).
         * @param buffers Array of pointers to the buffers.
         * @param nCh Number of channels to allocate.
         * @param nSamples Number of samples to allocate.
         */
        void AllocateBuffers(float**& buffers, unsigned nCh, unsigned nSamples);

        /** Deallocate internal 2D buffers.
         * @param buffers Buffers to deallocate.
         * @param nCh Number of channels in the buffer.
         */
        void DeallocateBuffers(float**& buffers, unsigned nCh);
    };

} // namespace spaudio

#endif // SPATIALAUDIO_RENDERER_H

/*############################################################################*/
/*#                                                                          #*/
/*#  Pan mono objects on a speaker layout in real-time.                      #*/
/*#  Copyright © 2025 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      ObjectPanner.cpp                                         #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          10/09/2025                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/


#pragma once

#include <memory>

#include "SpatialaudioAPI.h"
#include "PointSourcePannerGainCalc.h"
#include "GainInterp.h"
#include "LoudspeakerLayouts.h"
#include "Coordinates.h"

namespace spaudio {

    /** Pan a point source on a loudspeaker layout in real-time.
     */
    class SPAUDIO_API ObjectPanner
    {
    public:
        ObjectPanner();
        ~ObjectPanner();

         /** Re-create the object for the given configuration. Previous data is
          *  lost. Returns true if successful.
         * @param layout            The output loudspeaker layout.
         * @param sampleRate        Sample rate of the signal to be processed.
         * @param fadeTimeMilliSec  Fade time in milliseconds when the position changes.
         * @return
         */
        bool Configure(OutputLayout layout, unsigned sampleRate, float fadeTimeMilliSec);

        /** Reset the state of the panner. */
        void Reset();

        /** Get the number of speakers in the layout set in Configure. */
        int GetNumSpeakers();

        /** Set the position of the source.
         * @param polPosition   New polar position.
         */
        void SetPosition(const PolarPosition<double>& polPosition);

        /** Pan the mono input to the loudspeaker layout
         * @param pfSrc       Pointer to the signal to pan.
         * @param nSamplesIn  The number of samples to pan.
         * @param ppDst       Output containing pIn multiplied by the gain vector.
         * @param nSamplesOut The number of samples in the output buffer. Must be at least nSamplesIn + nOffset.
         * @param nOffset     Optional offset position when writing to the output.
         */
        void Process(float* pIn, unsigned nSamplesIn, float** ppDst, unsigned int nSamplesOut, unsigned int nOffset = 0);

        /** Pan a mono input to a loudspeaker layout and add it to the contents in ppDst.
         *  Allows an optional offset for the position in samples at which the output is to be written.
         * @param pfSrc       Pointer to the signal to pan.
         * @param nSamplesIn  The number of samples to pan.
         * @param ppDst       Output containing its original content plus pIn multiplied by the gain vector.
         * @param nSamplesOut The number of samples in the output buffer. Must be at least nSamplesIn + nOffset
         * @param nOffset     Optional offset position when writing to the output.
         * @param fGain       Optional gain to apply to the output.
         */
        void ProcessAccumul(float* pIn, unsigned nSamplesIn, float** ppDst, unsigned int nSamplesOut, unsigned int nOffset = 0, float fGain = 1.f);

    private:
        // The current panning gains without the LFE channel(s) calculated by the point source panner
        std::vector<double> m_gainsTmp;
        // The current panning gains with zero padding for the LFE channel(s)
        std::vector<double> m_gains;

        // Output loudspeaker layout
        Layout m_layout;

        // Point source panner to calculate the gains
        std::unique_ptr<PointSourcePannerGainCalc> m_pspGainCalc;

        // The time to fade from the previous gains to the target ones
        float m_fadingTimeMilliSec = 0.f;
        unsigned int m_fadingSamples = 0;

        GainInterp<double> m_coeffInterp;

        void insertLFE(const std::vector<double>& inGains, std::vector<double>& outGains);
    };

} // namespace spaudio

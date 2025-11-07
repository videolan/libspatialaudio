/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  AmbisonicEncoderDist - Ambisonic Encoder with distance                 #*/
/*#  Copyright © 2007 Aristotel Digenis                                      #*/
/*#                                                                          #*/
/*#  Filename:      AmbisonicEncoderDist.h                                   #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          19/05/2007                                               #*/
/*#  Author(s):     Aristotel Digenis                                        #*/
/*#  Licence:       MIT                                                      #*/
/*#                                                                          #*/
/*############################################################################*/


#ifndef _AMBISONIC_ENCODER_DIST_H
#define _AMBISONIC_ENCODER_DIST_H

#include "SpatialaudioAPI.h"
#include "AmbisonicEncoder.h"

namespace spaudio {

    const unsigned knSpeedOfSound = 344;
    const unsigned knMaxDistance = 150;

    /// Ambisonic encoder with distance cues.

    /** This is similar to a normal the ambisonic encoder, but takes the source's
        distance into account, delaying the signal, adjusting its gain, and
        implementing "W-Panning"(interior effect). If distance is not an issue,
        then use AmbisonicEncoder which is more efficient. */

    class SPAUDIO_API AmbisonicEncoderDist : public AmbisonicEncoder
    {
    public:
        AmbisonicEncoderDist();
        ~AmbisonicEncoderDist();

        /** Re-create the object for the given configuration. Previous data is
         *  lost. Returns true if successful.
         * @param nOrder        Order to which the signal should be encoded.
         * @param b3D           Flag if the 2D or 3D encoding.
         * @param nSampleRate   Sample rate of the signal to be encoded.
         * @return              Returns true if successfully configured.
         */
        virtual bool Configure(unsigned nOrder, bool b3D, unsigned nSampleRate) override;

        /** Resets members such as delay lines. */
        virtual void Reset() override;

        /** Refreshes coefficients. */
        virtual void Refresh() override;

        /** Encode mono stream to B-Format.
         * @param pfSrc     Pointer to mono signal to be encoded.
         * @param nSamples  Number of samples to encode.
         * @param pBFDst    Output B-Format signal.
         */
        void Process(float* pfSrc, unsigned nSamples, BFormat* pBFDst);

        /** Set the radius of the intended playback speaker setup which is used for
         *  the interior effect (W-Panning).
         * @param fRoomRadius   Room radius in metres
         */
        void SetRoomRadius(float fRoomRadius);

        /** Returns the radius of the intended playback speaker setup, which is
         *  used for the interior effect (W-Panning).
         * @return  Room radius in metres.
         */
        float GetRoomRadius();

    protected:
        unsigned m_nSampleRate;
        float m_fDelay;
        int m_nDelay;
        unsigned m_nDelayBufferLength;
        float* m_pfDelayBuffer;
        int m_nIn;
        int m_nOutA;
        int m_nOutB;
        float m_fRoomRadius;
        float m_fInteriorGain;
        float m_fExteriorGain;
    };

} // namespace spaudio

#endif // _AMBISONIC_ENCODER_DIST_H

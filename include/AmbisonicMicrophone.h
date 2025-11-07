/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  AmbisonicMicrophone - Ambisonic Microphone                             #*/
/*#  Copyright © 2007 Aristotel Digenis                                      #*/
/*#                                                                          #*/
/*#  Filename:      AmbisonicMicrophone.h                                    #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          19/05/2007                                               #*/
/*#  Author(s):     Aristotel Digenis                                        #*/
/*#  Licence:       MIT                                                      #*/
/*#                                                                          #*/
/*############################################################################*/


#ifndef _AMBISONIC_MICROPHONE_H
#define _AMBISONIC_MICROPHONE_H

#include "SpatialaudioAPI.h"
#include "AmbisonicSource.h"
#include "BFormat.h"

namespace spaudio {

    /// Ambisonic microphone

    /** This is a microphone class. It is similar to ::AmbisonicSpeaker, with the
        addition of having directivity control. */

    class SPAUDIO_API AmbisonicMicrophone : public AmbisonicSource
    {
    public:
        AmbisonicMicrophone();
        ~AmbisonicMicrophone();

        /** Recalculate coefficients, and apply normalisation factors. */
        void Refresh();

        /** Decode B-Format to microphone feed.
         * @param pBFSrc    BFormat scene to be sampled by the microphone directivity.
         * @param nSamples  Number of samples to process.
         * @param pfDst     Mono microphone signal.
         */
        void Process(BFormat* pBFSrc, unsigned nSamples, float* pfDst);

        /** Set the microphone's directivity.
         * @param fDirectivity  Microphone directivity.
         */
        void SetDirectivity(float fDirectivity);

        /** Get the microphone's directivity.
         * @return  Microphone directivity.
         */
        float GetDirectivity();

    protected:
        float m_fDirectivity;
    };

} // namespace spaudio

#endif // _AMBISONIC_MICROPHONE_H

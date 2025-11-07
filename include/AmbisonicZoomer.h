/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  AmbisonicZoomer - Ambisonic Zoomer                                     #*/
/*#  Copyright © 2007 Aristotel Digenis                                      #*/
/*#  Copyright © 2017 Videolabs                                              #*/
/*#                                                                          #*/
/*#  Filename:      AmbisonicZoomer.h                                        #*/
/*#  Version:       0.2                                                      #*/
/*#  Date:          19/05/2007                                               #*/
/*#  Author(s):     Aristotel Digenis, Peter Stitt                           #*/
/*#  Licence:       LGPL (+ Proprietary)                                     #*/
/*#                                                                          #*/
/*############################################################################*/


#ifndef _AMBISONIC_ZOOMER_H
#define _AMBISONIC_ZOOMER_H

#include "SpatialaudioAPI.h"
#include "AmbisonicBase.h"
#include "AmbisonicDecoder.h"
#include "BFormat.h"

#include <memory>

namespace spaudio {

    /// Ambisonic zoomer.

    /** This object is used to apply a zoom effect into BFormat soundfields. */

    class SPAUDIO_API AmbisonicZoomer : public AmbisonicBase
    {
    public:
        AmbisonicZoomer();
        virtual ~AmbisonicZoomer() = default;

        /** Re-create the object for the given configuration. Previous data is
         *  lost. The last argument is not used, it is just there to match with
         *  the base class's form. Returns true if successful.
         * @param nOrder        Order of the signal to process.
         * @param b3D           Flag if the signal is 3D.
         * @param nBlockSize    Number of samples to process each block.
         * @param sampleRate    Sample rate of the signal to process.
         * @return              Returns true if successfully configured.
         */
        virtual bool Configure(unsigned nOrder, bool b3D, unsigned nBlockSize, unsigned sampleRate);

        /** Not implemented. */
        void Reset();

        /** Recalculate coefficients. */
        void Refresh();

        /** Set zoom factor. This is in a range from 0 to 1, with 0 being no zoom,
         *  1 full forward zoom.
         * @param fZoom     How much zoom to apply.
         */
        void SetZoom(float fZoom);

        /** Get zoom factor.
         * @return      Zoom factor
         */
        float GetZoom();

        /** Zoom into B-Format stream.
         * @param pBFSrcDst     B-Format signal to process.
         * @param nSamples      Number of samples to process.
         */
        void Process(BFormat* pBFSrcDst, unsigned nSamples);

    private:
        using AmbisonicBase::Configure;
    protected:
        AmbisonicDecoder m_AmbDecoderFront;

        std::unique_ptr<float[]> m_AmbEncoderFront;
        std::unique_ptr<float[]> m_AmbEncoderFront_weighted;
        std::unique_ptr<float[]> a_m;

        float m_fZoom;
        float m_fZoomRed;
        float m_AmbFrontMic;
        float m_fZoomBlend;

        /** Compute factorial of integer.
         * @param M     Interger input.
         * @return      Factorial of input as a float.
         */
        float factorial(unsigned M);
    };

} // namespace spaudio

#endif // _AMBISONIC_ZOOMER_H

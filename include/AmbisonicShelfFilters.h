/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  AmbisonicShelfFilters - Ambisonic psychoactic optimising filters       #*/
/*#  Copyright © 2020 Videolabs                                              #*/
/*#                                                                          #*/
/*#  Filename:      AmbisonicShelfFilters.h                                  #*/
/*#  Version:       0.2                                                      #*/
/*#  Date:          23/03/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL                                                     #*/
/*#                                                                          #*/
/*############################################################################*/


#ifndef _AMBISONIC_SHELF_FILTERS_H
#define    _AMBISONIC_SHELF_FILTERS_H

#include "SpatialaudioAPI.h"
#include "AmbisonicBase.h"
#include "BFormat.h"
#include "kiss_fft/kiss_fftr.h"

namespace spaudio {

    /** This class applies frequency dependent basic & max-rE optimisation to a B-format signal using linear phase FIR filters.
     *  This class is deprecated in favour of AmbisonicOptimFilters, which uses LinkwitzRiley IIR filters to apply frequency dependent gains.
     */
    class SPAUDIO_API
        [[deprecated("This class is deprecated. Please use AmbisonicOptimFilters class instead.")]]
    AmbisonicShelfFilters : public AmbisonicBase
    {
    public:
        AmbisonicShelfFilters();
        ~AmbisonicShelfFilters();

        /** Re-create the object for the given configuration. Previous data is
         *  lost. The last argument is not used, it is just there to match with
         *  the base class's form. Returns true if successful.
         * @param nOrder        Order of the signal to be filtered.
         * @param b3D           Flag if the signal is 3D.
         * @param nBlockSize    Maximum number of samples per block.
         * @param nMisc         Unused.
         * @return              Returns true if successfully configured.
         */
        bool Configure(unsigned nOrder, bool b3D, unsigned nBlockSize, unsigned nMisc);

        /** Reset the internal buffers. */
        void Reset();

        /** Not implemented. */
        void Refresh();

        /** Filter B-Format stream.
         *  Overload with number of samples (nSamples < m_nBlockSize) to process shorter block sizes
         * @param pBFSrcDst     The signal to be filtered.
         */
        void Process(BFormat* pBFSrcDst);
        void Process(BFormat* pBFSrcDst, unsigned int nSamples);

    private:
        using AmbisonicBase::Configure;

    protected:
        kiss_fftr_cfg m_pFFT_psych_cfg;
        kiss_fftr_cfg m_pIFFT_psych_cfg;

        float* m_pfScratchBufferA;
        float** m_pfOverlap;
        unsigned m_nFFTSize;
        unsigned m_nBlockSize;
        unsigned m_nTaps;
        unsigned m_nOverlapLength;
        unsigned m_nFFTBins;
        float m_fFFTScaler;

        kiss_fft_cpx** m_ppcpPsychFilters;
        kiss_fft_cpx* m_pcpScratch;
    };

} // namespace spaudio

#endif // _AMBISONIC_SHELF_FILTERS_H

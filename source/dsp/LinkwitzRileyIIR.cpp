/*############################################################################*/
/*#                                                                          #*/
/*#  A simple Linkwitz-Riley IIR filter                                      #*/
/*#                                                                          #*/
/*#                                                                          #*/
/*#  Filename:      LinkwitzRileyIIR.cpp                                     #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          03/04/2024                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#include "dsp/LinkwitzRileyIIR.h"
#include <cmath>

namespace spaudio {

    LinkwitzRileyIIR::LinkwitzRileyIIR()
    {
    }

    LinkwitzRileyIIR::~LinkwitzRileyIIR()
    {
    }

    bool LinkwitzRileyIIR::Configure(unsigned int nCh, unsigned int sampleRate, float crossoverFreq)
    {
        for (int i = 0; i < 2; ++i)
        {
            bool success = m_lp[i].Configure(nCh, sampleRate, crossoverFreq, std::sqrt(0.5f), IIRFilter::FilterType::LowPass);
            if (!success)
                return false;
            success = m_hp[i].Configure(nCh, sampleRate, crossoverFreq, std::sqrt(0.5f), IIRFilter::FilterType::HighPass);
            if (!success)
                return false;
        }

        Reset();

        return true;
    }

    void LinkwitzRileyIIR::Reset()
    {
        for (int i = 0; i < 2; ++i)
        {
            m_lp[i].Reset();
            m_hp[i].Reset();
        }
    }

    void LinkwitzRileyIIR::Process(float** pIn, float** pOutLP, float** pOutHP, unsigned int nSamples)
    {
        m_lp[0].Process(pIn, pOutLP, nSamples);
        m_lp[1].Process(pOutLP, pOutLP, nSamples);

        m_hp[0].Process(pIn, pOutHP, nSamples);
        m_hp[1].Process(pOutHP, pOutHP, nSamples);
    }

} // namespace spaudio

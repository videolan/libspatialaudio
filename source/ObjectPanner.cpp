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

#include "ObjectPanner.h"

#include <assert.h>

namespace spaudio {

    ObjectPanner::ObjectPanner() : m_coeffInterp(0)
    {
    }

    ObjectPanner::~ObjectPanner()
    {
    }

    bool ObjectPanner::Configure(OutputLayout layout, unsigned sampleRate, float fadeTimeMilliSec)
    {
        if (fadeTimeMilliSec < 0.f)
            return false;

        m_layout = Layout::getMatchingLayout(layout);

        m_pspGainCalc = std::make_unique<PointSourcePannerGainCalc>(layout);

        m_gainsTmp.resize(m_pspGainCalc->getNumChannels());
        m_gains.resize(m_layout.getNumChannels());
        m_coeffInterp = GainInterp<double>(static_cast<int>(m_gains.size()));

        m_fadingTimeMilliSec = fadeTimeMilliSec;
        m_fadingSamples = (unsigned)std::round(0.001f * m_fadingTimeMilliSec * (float)sampleRate);

        return true;
    }

    void ObjectPanner::Reset()
    {
        m_coeffInterp.Reset();
    }

    int ObjectPanner::GetNumSpeakers()
    {
        return static_cast<int>(m_layout.getNumChannels());
    }

    void ObjectPanner::SetPosition(const PolarPosition<double>& polPosition)
    {
        m_pspGainCalc->CalculateGains(polPosition, m_gainsTmp);
        insertLFE(m_gainsTmp, m_gains);
        m_coeffInterp.SetGainVector(m_gains, m_fadingSamples);
    }

    void ObjectPanner::Process(float* pfSrc, unsigned nSamplesIn, float** ppDst, unsigned int nSamplesOut, unsigned int nOffset)
    {
        assert(nSamplesIn + nOffset <= nSamplesOut); // Cannot write beyond the of the output buffers!

        m_coeffInterp.Process(pfSrc, ppDst, nSamplesIn, nOffset);
    }

    void ObjectPanner::ProcessAccumul(float* pfSrc, unsigned nSamplesIn, float** ppDst, unsigned int nSamplesOut, unsigned int nOffset, float fGain)
    {
        assert(nSamplesIn + nOffset <= nSamplesOut); // Cannot write beyond the of the output buffers!

        m_coeffInterp.ProcessAccumul(pfSrc, ppDst, nSamplesIn, nOffset, fGain);
    }
    void ObjectPanner::insertLFE(const std::vector<double>& inGains, std::vector<double>& outGains)
    {
        if (!m_layout.hasLfe()) // No LFE to insert so just copy the gain vector
        {
            outGains = inGains;
            return;
        }

        int iCount = 0;
        for (int i = 0; i < m_layout.getNumChannels(); ++i)
            if (!m_layout.getChannel(i).getIsLfe())
                outGains[i] = inGains[iCount++];
            else
                outGains[i] = 0.;
    }
}

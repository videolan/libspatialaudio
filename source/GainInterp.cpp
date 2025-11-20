/*############################################################################*/
/*#                                                                          #*/
/*#  Apply a vector of gains to a mono input with interpolation              #*/
/*#                                                                          #*/
/*#  Filename:      GainInterp.cpp		                                     #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          30/10/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#include "GainInterp.h"

#include <assert.h>
#include <cstddef>
#include <cmath>

namespace spaudio {

    template<typename T>
    GainInterp<T>::GainInterp(unsigned int nCh) : m_currentGainVec(nCh), m_targetGainVec(nCh), m_targetGainVecTmp(nCh), m_deltaGainVec(nCh)
    {

    }

    template<typename T>
    GainInterp<T>::~GainInterp()
    {
    }

    template<typename T>
    void GainInterp<T>::SetGainValue(T newGainVal, unsigned int interpTimeInSamples)
    {
        for (auto& g : m_targetGainVecTmp)
            g = newGainVal;
        SetGainVector(m_targetGainVecTmp, interpTimeInSamples);
    }

    template<typename T>
    void GainInterp<T>::SetGainVector(const std::vector<T>& newGainVec, unsigned int interpTimeInSamples)
    {
        assert(newGainVec.size() == m_targetGainVec.size()); //Number of channels must match!

        if (m_targetGainVec != newGainVec)
        {
            if (interpTimeInSamples > 0)
            {
                m_targetGainVec = newGainVec;

                for (size_t i = 0; i < m_targetGainVec.size(); ++i)
                    m_deltaGainVec[i] = (m_targetGainVec[i] - m_currentGainVec[i]) / static_cast<T>(interpTimeInSamples);

                m_interpDurInSamples = interpTimeInSamples;
                // Reset the interpolation counter to start interpolation
                m_iInterpCount = 0;
            }
            else
            {
                // If smoothing time is zero samples then set current and target vectors to the current value and do not interpolate
                m_targetGainVec = newGainVec;
                m_currentGainVec = newGainVec;
                for (auto& g : m_deltaGainVec)
                    g = static_cast<T>(0.);
                m_interpDurInSamples = interpTimeInSamples;
                m_iInterpCount = m_interpDurInSamples;
            }
        }
    }

    template<typename T>
    void GainInterp<T>::Process(const float* pIn, float** ppOut, unsigned int nSamples, unsigned int nOffset)
    {
        if (m_isFirstCall)
        {
            Reset();
            m_isFirstCall = false;
        }

        unsigned int nCh = (unsigned int)m_targetGainVec.size();
        // The number of samples to interpolate over in this block
        unsigned int nInterpSamples = std::min(nSamples, m_interpDurInSamples - m_iInterpCount);

        if (m_iInterpCount < m_interpDurInSamples)
        {
            for (unsigned int iCh = 0; iCh < nCh; ++iCh)
                for (unsigned int i = 0; i < nInterpSamples; ++i)
                {
                    ppOut[iCh][i + nOffset] = pIn[i] * static_cast<float>(m_currentGainVec[iCh]);
                    m_currentGainVec[iCh] += m_deltaGainVec[iCh];
                }

            m_iInterpCount += nInterpSamples;
        }

        for (unsigned int iCh = 0; iCh < nCh; ++iCh)
        {
            float gain = static_cast<float>(m_targetGainVec[iCh]);
            if (std::abs(gain) < 1e-5f)
                continue;

            for (unsigned int i = nInterpSamples; i < nSamples; ++i)
                ppOut[iCh][i + nOffset] = pIn[i] * gain;
        }
    }

    template<typename T>
    void GainInterp<T>::ProcessAccumul(const float* pIn, float** ppOut, unsigned int nSamples, unsigned int nOffset, T gain)
    {
        if (m_isFirstCall)
        {
            Reset();
            m_isFirstCall = false;
        }

        unsigned int nCh = (unsigned int)m_targetGainVec.size();
        // The number of samples to interpolate over in this block
        unsigned int nInterpSamples = std::min(nSamples, m_interpDurInSamples - m_iInterpCount);

        if (m_iInterpCount < m_interpDurInSamples)
        {
            for (unsigned int iCh = 0; iCh < nCh; ++iCh)
                for (unsigned int i = 0; i < nInterpSamples; ++i)
                {
                    ppOut[iCh][i + nOffset] += pIn[i] * static_cast<float>(m_currentGainVec[iCh] * gain);
                    m_currentGainVec[iCh] += m_deltaGainVec[iCh];
                }

            m_iInterpCount += nInterpSamples;
        }

        for (unsigned int iCh = 0; iCh < nCh; ++iCh)
        {
            float gain = static_cast<float>(m_targetGainVec[iCh]);
            if (std::abs(gain) < 1e-5f)
                continue;

            for (unsigned int i = nInterpSamples; i < nSamples; ++i)
                ppOut[iCh][i + nOffset] += pIn[i] * gain;
        }
    }

    template<typename T>
    void GainInterp<T>::Reset()
    {
        m_iInterpCount = m_interpDurInSamples;
        m_currentGainVec = m_targetGainVec;
        m_isFirstCall = true;
    }

    template class GainInterp<float>;
    template class GainInterp<double>;

} // namespace spaudio

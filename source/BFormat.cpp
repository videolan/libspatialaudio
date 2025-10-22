/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  BFormat - Ambisonic BFormat                                            #*/
/*#  Copyright © 2007 Aristotel Digenis                                      #*/
/*#                                                                          #*/
/*#  Filename:      BFormat.cpp                                              #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          19/05/2007                                               #*/
/*#  Author(s):     Aristotel Digenis                                        #*/
/*#  Licence:       MIT                                                      #*/
/*#                                                                          #*/
/*############################################################################*/


#include "BFormat.h"
#include <cassert>

namespace spaudio {

    BFormat::BFormat()
    {
        m_nSamples = 0;
        m_nDataLength = 0;
    }

    unsigned BFormat::GetSampleCount()
    {
        return m_nSamples;
    }

    bool BFormat::Configure(unsigned nOrder, bool b3D, unsigned nSampleCount)
    {
        bool success = AmbisonicBase::Configure(nOrder, b3D, nSampleCount);
        if (!success)
            return false;

        m_nSamples = nSampleCount;
        m_nDataLength = m_nSamples * m_nChannelCount;

        m_pfData.resize(m_nDataLength);
        memset(m_pfData.data(), 0, m_nDataLength * sizeof(float));
        m_ppfChannels.reset(new float* [m_nChannelCount]);

        for (unsigned niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            m_ppfChannels[niChannel] = &m_pfData[niChannel * m_nSamples];
        }

        return true;
    }

    void BFormat::Reset()
    {
        memset(m_pfData.data(), 0, m_nDataLength * sizeof(float));
    }

    void BFormat::Refresh()
    {

    }

    void BFormat::InsertStream(float* pfData, unsigned nChannel, unsigned nSamples)
    {
        memcpy(m_ppfChannels[nChannel], pfData, nSamples * sizeof(float));
    }

    void BFormat::AddStream(float* pfData, unsigned nChannel, unsigned nSamples, unsigned nOffset, float gain)
    {
        unsigned niSample = 0;

        for (niSample = 0; niSample < nSamples; niSample++)
        {
            m_ppfChannels[nChannel][niSample + nOffset] += pfData[niSample] * gain;
        }
    }

    void BFormat::ExtractStream(float* pfData, unsigned nChannel, unsigned nSamples)
    {
        memcpy(pfData, m_ppfChannels[nChannel], nSamples * sizeof(float));
    }

    float* BFormat::GetChannelPointer(unsigned nChannel)
    {
        return m_ppfChannels[nChannel];
    }

    void BFormat::operator = (const BFormat& bf)
    {
        assert(bf.m_pfData.size() <= m_pfData.size());
        unsigned dataLength = std::min(m_nDataLength, bf.m_nDataLength);
        memcpy(m_pfData.data(), bf.m_pfData.data(), dataLength * sizeof(float));
    }

    bool BFormat::operator == (const BFormat& bf)
    {
        if (m_b3D == bf.m_b3D && m_nOrder == bf.m_nOrder && m_nDataLength == bf.m_nDataLength)
            return true;
        else
            return false;
    }

    bool BFormat::operator != (const BFormat& bf)
    {
        if (m_b3D != bf.m_b3D || m_nOrder != bf.m_nOrder || m_nDataLength != bf.m_nDataLength)
            return true;
        else
            return false;
    }

    BFormat& BFormat::operator += (const BFormat& bf)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] += bf.m_ppfChannels[niChannel][niSample];
            }
        }

        return *this;
    }

    BFormat& BFormat::operator -= (const BFormat& bf)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] -= bf.m_ppfChannels[niChannel][niSample];
            }
        }

        return *this;
    }

    BFormat& BFormat::operator *= (const BFormat& bf)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] *= bf.m_ppfChannels[niChannel][niSample];
            }
        }

        return *this;
    }

    BFormat& BFormat::operator /= (const BFormat& bf)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] /= bf.m_ppfChannels[niChannel][niSample];
            }
        }

        return *this;
    }

    BFormat& BFormat::operator += (const float& fValue)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] += fValue;
            }
        }

        return *this;
    }

    BFormat& BFormat::operator -= (const float& fValue)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] -= fValue;
            }
        }

        return *this;
    }

    BFormat& BFormat::operator *= (const float& fValue)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] *= fValue;
            }
        }

        return *this;
    }

    BFormat& BFormat::operator /= (const float& fValue)
    {
        unsigned niChannel = 0;
        unsigned niSample = 0;
        for (niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for (niSample = 0; niSample < m_nSamples; niSample++)
            {
                m_ppfChannels[niChannel][niSample] /= fValue;
            }
        }

        return *this;
    }

} // namespace spaudio

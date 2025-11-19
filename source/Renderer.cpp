/*############################################################################*/
/*#                                                                          #*/
/*#  A renderer for Object, HOA and DirectSpeaker streams.                   #*/
/*#  Renderer                                                                #*/
/*#  Copyright © 2020 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      Renderer.cpp                                             #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#include "Renderer.h"
#include<type_traits>
#include<iostream>

namespace spaudio {

    Renderer::Renderer()
    {
        m_RenderLayout = OutputLayout::Stereo;
        m_nSamples = 0;
    }

    Renderer::~Renderer()
    {
        DeallocateBuffers(m_speakerOut, m_nChannelsToRender);
        DeallocateBuffers(m_speakerOutDirect, m_nChannelsToRender);
        DeallocateBuffers(m_speakerOutDiffuse, m_nChannelsToRender);
        DeallocateBuffers(m_virtualSpeakerOut, m_nChannelsToRender);
        DeallocateBuffers(m_binauralOut, 2);
    }

    bool Renderer::Configure(OutputLayout outputTarget, unsigned int hoaOrder, unsigned int nSampleRate, unsigned int nSamples, const StreamInformation& channelInfo, std::string HRTFPath, bool useLfeBinaural, Optional<Screen> reproductionScreen, const std::vector<PolarPosition<double>>& layoutPositions)
    {
        // Set the output layout
        m_RenderLayout = outputTarget;
        // Set the order to be used for the HOA rendering
        m_HoaOrder = hoaOrder;
        m_nAmbiChannels = (m_HoaOrder + 1) * (m_HoaOrder + 1);
        if (m_HoaOrder > 3)
            return false; // only accepts orders 0 to 3
        // Set the maximum number of samples expected in a frame
        m_nSamples = nSamples;
        // Store the channel information
        m_channelInformation = channelInfo;
        // Configure the B-format buffers
        bool bHoaOutConfig = m_hoaAudioOut.Configure(hoaOrder, true, nSamples);
        if (!bHoaOutConfig)
            return false;

        // Set up the output layout
        switch (m_RenderLayout)
        {
        case OutputLayout::Stereo:
            m_outputLayout = Layout::getMatchingLayout("0+2+0");
            break;
        case OutputLayout::Quad:
            m_outputLayout = Layout::getMatchingLayout("0+4+0");
            break;
        case OutputLayout::FivePointOne:
            m_outputLayout = Layout::getMatchingLayout("0+5+0");
            break;
        case OutputLayout::SevenPointOne:
            m_outputLayout = Layout::getMatchingLayout("0+7+0");
            break;
        case OutputLayout::FivePointOnePointTwo:
            m_outputLayout = Layout::getMatchingLayout("2+5+0");
            break;
        case OutputLayout::FivePointOnePointFour:
            m_outputLayout = Layout::getMatchingLayout("4+5+0");
            break;
        case OutputLayout::FivePointOnePointFourPlusLow:
            m_outputLayout = Layout::getMatchingLayout("4+5+1");
            break;
        case OutputLayout::SevenPointOnePointThree:
            m_outputLayout = Layout::getMatchingLayout("3+7+0");
            break;
        case OutputLayout::ThirteenPointOne:
            m_outputLayout = Layout::getMatchingLayout("4+9+0");
            break;
        case OutputLayout::TwentyTwoPointTwo:
            m_outputLayout = Layout::getMatchingLayout("9+10+3");
            break;
        case OutputLayout::SevenPointOnePointFour:
            m_outputLayout = Layout::getMatchingLayout("4+7+0");
            break;
        case OutputLayout::BEAR_9_10_5:
            m_outputLayout = Layout::getMatchingLayout("9+10+5");
            break;
        case OutputLayout::SevenPointOnePointTwo:
            m_outputLayout = Layout::getMatchingLayout("2+7+0");
            break;
        case OutputLayout::ThreePointOnePointTwo:
            m_outputLayout = Layout::getMatchingLayout("2+3+0");
            break;
        case OutputLayout::Binaural:
            // Render to the BEAR layout and before binauralising
            m_outputLayout = Layout::getLayoutWithoutLFE(Layout::getMatchingLayout("9+10+5"));
            break;
        default:
            break;
        }

        // If specified, set the layout positions
        if (layoutPositions.size() > 0 && layoutPositions.size() != m_outputLayout.getNumChannels())
            return false; // If setting custom loudspeaker positions the sizes must match.
        else if (layoutPositions.size() > 0)
        {
            for (int iLdspk = 0; iLdspk < (int)m_outputLayout.getNumChannels(); ++iLdspk)
                m_outputLayout.getChannel(iLdspk).setPolarPosition(layoutPositions[iLdspk]);
        }

        // Check the layout coordinates are within range for the specified layout
        if (!checkLayoutAngles(m_outputLayout))
            return false; // At least one loudspeaker is out of range!

        m_nChannelsToRender = (unsigned int)m_outputLayout.getNumChannels();
        m_nChannelsToOutput = m_nChannelsToRender;

        if (reproductionScreen.hasValue())
        {
            Screen screen = reproductionScreen.value();
            m_outputLayout.setReproductionScreen(screen);
            m_objMetaDataTmp.referenceScreen = screen;
        }

        // Clear the vectors containing the HOA and panning objects so that if the renderer is
        // reconfigured the mappings will be correct
        m_hoaEncoders.clear();
        m_pannerTrackInd.clear();
        m_objectMetadata.clear();
        m_channelToObjMap.clear();

        // Set up required processors based on channelInfo
        unsigned iObj = 0;
        unsigned iDirSpk = 0;
        unsigned iHOA = 0;
        for (unsigned int iCh = 0; iCh < channelInfo.nChannels; ++iCh)
        {
            switch (channelInfo.typeDefinition[iCh])
            {
            case TypeDefinition::DirectSpeakers:
                m_pannerTrackInd.push_back({ iCh,TypeDefinition::DirectSpeakers });
                m_directSpeakerGainInterp.push_back(GainInterp<double>(m_nChannelsToRender));
                m_channelToDirectSpeakerMap.insert(std::pair<int, int>(iCh, iDirSpk++));
                break;
            case TypeDefinition::Matrix:
                break;
            case TypeDefinition::Objects:
                m_pannerTrackInd.push_back({ iCh,TypeDefinition::Objects });
                m_gainInterpDirect.push_back(GainInterp<double>(m_nChannelsToRender));
                m_gainInterpDiffuse.push_back(GainInterp<double>(m_nChannelsToRender));
                m_objectMetadata.push_back(ObjectMetadata());
                if (reproductionScreen.hasValue())
                    m_objectMetadata.back().referenceScreen = reproductionScreen.value();
                m_channelToObjMap.insert(std::pair<int, int>(iCh, iObj++));
                break;
            case TypeDefinition::HOA:
                ++iHOA;
                break;
            case TypeDefinition::Binaural:
                break;
            default:
                break;
            }
        }

        if (iHOA > 0 && iHOA != m_nAmbiChannels)
            return false; // Either the HOA stream in channelInfo is of an order that doesn't match hoaOrder or there is more than one HOA stream.

        // Set the DirectSpeaker gain interpolation time for when the metadata gain varies.
        // Smooth over a single full frame of audio.
        m_gainInterpTime = nSamples;

        // Set up the gain calculator
        m_objectGainCalc = std::make_unique<adm::ObjectGainCalculator>(m_outputLayout);
        //Set up the direct gain calculator if output is not binaural
        m_directSpeakerGainCalc = std::make_unique<adm::DirectSpeakersGainCalc>(m_outputLayout);
        // Set up the decorrelator
        bool bDecorConfig = m_decorrelate.Configure(m_outputLayout, nSamples);
        if (!bDecorConfig)
            return false;

        // AllRAD decoder for HOA signals
        bool bHoaDecoderConfig = m_hoaDecoder.Configure(hoaOrder, nSamples, nSampleRate, m_outputLayout.getLayoutName(), m_outputLayout.hasLfe());
        if (!bHoaDecoderConfig)
            return false;

        if (m_RenderLayout == OutputLayout::Binaural)
        {
            m_useLfeBinaural = useLfeBinaural;
            for (int iLdspk = 0; iLdspk < (int)m_outputLayout.getNumChannels(); ++iLdspk)
            {
                auto pos = m_outputLayout.getChannel(iLdspk).getPolarPosition();
                m_hoaEncoders.push_back(AmbisonicEncoder());
                m_hoaEncoders[iLdspk].Configure(hoaOrder, true, nSampleRate, 0);
                m_hoaEncoders[iLdspk].SetPosition(PolarPosition<float>{ DegreesToRadians((float)pos.azimuth), DegreesToRadians((float)pos.elevation), 1.f });
            }

            bool bBinRot = m_hoaRotate.Configure(hoaOrder, true, nSamples, nSampleRate, 50.f);
            if (!bBinRot)
                return false;

            unsigned int tailLength = 0;
            bool bBinConf = m_hoaBinaural.Configure(hoaOrder, true, nSampleRate, nSamples, tailLength, HRTFPath);
            if (!bBinConf)
                return false;

            m_nChannelsToOutput = 2;

            AllocateBuffers(m_binauralOut, 2, nSamples);
        }

        // Set up the buffers holding the direct and diffuse speaker signals
        AllocateBuffers(m_speakerOut, m_nChannelsToRender, nSamples);
        AllocateBuffers(m_speakerOutDirect, m_nChannelsToRender, nSamples);
        AllocateBuffers(m_speakerOutDiffuse, m_nChannelsToRender, nSamples);
        AllocateBuffers(m_virtualSpeakerOut, m_nChannelsToRender, nSamples);

        // A buffer of zeros to use to clear the HOA buffer
        m_pZeros = std::make_unique<float[]>(nSamples);
        memset(m_pZeros.get(), 0, m_nSamples * sizeof(float));

        // Allocate vectors used during gain calculations
        m_directGains.resize(m_nChannelsToRender);
        m_diffuseGains.resize(m_nChannelsToRender);
        m_directSpeakerGains.resize(m_nChannelsToRender);

        // Set up the HOA gain interpolator
        m_hoaGainInterp.resize(m_nAmbiChannels, GainInterp<double>(1));
        for (auto& hoaGainInterp : m_hoaGainInterp)
            hoaGainInterp.SetGainValue(1.0, m_nSamples);

        // Set up the output gain interpolator
        m_outGainInterp.resize(m_nChannelsToOutput, GainInterp<double>(1));
        for (auto& outGainInterp : m_outGainInterp)
            outGainInterp.SetGainValue(1.0, 0);

        return true;
    }


    void Renderer::Reset()
    {
        m_decorrelate.Reset();
        m_hoaBinaural.Reset();
        m_hoaDecoder.Reset();
        ClearOutputBuffer();
        ClearObjectDirectBuffer();
        ClearObjectDiffuseBuffer();
        ClearHoaBuffer();

        for (size_t i = 0; i < m_gainInterpDirect.size(); ++i)
        {
            m_gainInterpDiffuse[i].Reset();
            m_gainInterpDirect[i].Reset();
        }

        for (auto& dirSpkGainInterp : m_directSpeakerGainInterp)
            dirSpkGainInterp.Reset();

        for (auto& hoaGainInterp : m_hoaGainInterp)
            hoaGainInterp.Reset();

        for (auto& outGainInterp : m_outGainInterp)
            outGainInterp.Reset();
    }

    unsigned int Renderer::GetSpeakerCount()
    {
        return m_RenderLayout == OutputLayout::Binaural ? 2 : (unsigned int)m_outputLayout.getNumChannels();
    }

    void Renderer::SetHeadOrientation(const RotationOrientation& newOrientation)
    {
        if (m_RenderLayout == OutputLayout::Binaural)
            m_hoaRotate.SetOrientation(newOrientation);
    }

    void Renderer::SetOutputGain(double outGain)
    {
        m_outGain = outGain;
        for (auto& outGainInterp : m_outGainInterp)
            outGainInterp.SetGainValue(m_outGain, m_nSamples);
    }

    void Renderer::AddObject(float* pIn, unsigned int nSamples, const ObjectMetadata& metadata, unsigned int nOffset)
    {
        // convert from cartesian to polar metadata (if required)
        adm::toPolar(metadata, m_objMetaDataTmp);

        // Map from the track index to the corresponding panner index
        int nObjectInd = GetMatchingIndex(m_pannerTrackInd, m_objMetaDataTmp.trackInd, TypeDefinition::Objects);

        if (nObjectInd == -1) // this track was not declared at construction. Stopping here.
        {
            std::cerr << "Renderer Error: Expected a track index that was declared an Object in construction. Input will not be rendered." << std::endl;
            return;
        }

        // Check if the metadata has changed
        int iObj = m_channelToObjMap[nObjectInd];
        bool newMetadata = !(m_objMetaDataTmp == m_objectMetadata[iObj]);
        if (newMetadata)
        {
            // Store the metadata
            m_objectMetadata[iObj] = m_objMetaDataTmp;

            if (m_RenderLayout == OutputLayout::Binaural) // Modify metadata based on EBU Tech 3396 Sec. 3.6.1.1
            {
                // The channelLock flag is cleared
                m_objMetaDataTmp.channelLock.reset();
                // Any zone entries are removed.
                m_objMetaDataTmp.zoneExclusion.resize(0);
            }

            // Calculate a new gain vector with this metadata
            m_objectGainCalc->CalculateGains(m_objMetaDataTmp, m_directGains, m_diffuseGains);

            // Get the interpolation time
            unsigned int interpLength = 0;
            if (m_objMetaDataTmp.jumpPosition.flag && m_objMetaDataTmp.jumpPosition.interpolationLength.hasValue())
                interpLength = m_objMetaDataTmp.jumpPosition.interpolationLength.value(); // = start_time + interpLen
            else if (m_objMetaDataTmp.jumpPosition.flag && !m_objMetaDataTmp.jumpPosition.interpolationLength.hasValue())
                interpLength = 0; // = start_time
            else
                interpLength = m_objMetaDataTmp.blockLength; // = end_time

            // Set the gains in the interpolators
            m_gainInterpDirect[iObj].SetGainVector(m_directGains, interpLength);
            m_gainInterpDiffuse[iObj].SetGainVector(m_diffuseGains, interpLength);
        }

        m_gainInterpDirect[iObj].ProcessAccumul(pIn, m_speakerOutDirect, nSamples, nOffset);
        m_gainInterpDiffuse[iObj].ProcessAccumul(pIn, m_speakerOutDiffuse, nSamples, nOffset);
    }

    void Renderer::AddHoa(float** pHoaIn, unsigned int nSamples, const HoaMetadata& metadata, unsigned int nOffset)
    {
        unsigned int nHoaCh = (unsigned int)metadata.orders.size();
        for (unsigned int iHoaCh = 0; iHoaCh < nHoaCh; ++iHoaCh)
        {
            int order = metadata.orders[iHoaCh];
            int degree = metadata.degrees[iHoaCh];
            // which HOA channel to write to based on the order and degree
            unsigned int iHoaChWrite = OrderAndDegreeToComponent(order, degree, true);
            float normConversionGain = 1.;
            if (compareCaseInsensitive(metadata.normalization, "N3D"))
                normConversionGain = N3dToSn3dFactor<float>(order);
            else if (compareCaseInsensitive(metadata.normalization, "FuMa"))
                normConversionGain = FuMaToSn3dFactor<float>(order, degree);
            m_hoaGainInterp[iHoaChWrite].SetGainValue(metadata.gain, m_gainInterpTime);
            m_hoaAudioOut.AddStream(pHoaIn[iHoaCh], iHoaChWrite, nSamples, nOffset, normConversionGain);
            float* ppOut[1] = { m_hoaAudioOut.GetChannelPointer(iHoaChWrite) };
            m_hoaGainInterp[iHoaChWrite].Process(m_hoaAudioOut.GetChannelPointer(iHoaChWrite), ppOut, nSamples, nOffset);
        }
    }

    void Renderer::AddDirectSpeaker(float* pDirSpkIn, unsigned int nSamples, const DirectSpeakerMetadata& metadata, unsigned int nOffset)
    {
        bool isSpeakerLFE = isLFE(metadata);
        if (m_RenderLayout == OutputLayout::Binaural && isSpeakerLFE && !m_useLfeBinaural)
            return; // Do not add LFE when rendering to binaural, according to EBU Tech 3396 Sec. 3.7.1

        if (m_RenderLayout == OutputLayout::Binaural) // Modify metadata based on EBU Tech 3396 Sec. 3.7.1
        {
            // Keep the metadata that will only use screen locking and the point source panner in m_directSpeakerGainCalc->calculateGains()
            m_dirSpkBinMetaDataTmp.speakerLabel = metadata.speakerLabel;
            m_dirSpkBinMetaDataTmp.channelFrequency = metadata.channelFrequency;
            m_dirSpkBinMetaDataTmp.polarPosition = metadata.polarPosition;
            m_dirSpkBinMetaDataTmp.screenEdgeLock = metadata.screenEdgeLock;
            m_dirSpkBinMetaDataTmp.trackInd = metadata.trackInd;

            if (isSpeakerLFE && m_useLfeBinaural) // Set the direction of the LFE channel to az = 0deg, el = -30deg
            {
                // The BEAR layout does not contain any LFE channels so set the LFE to B+000
                m_dirSpkBinMetaDataTmp.speakerLabel = "B+000";
                m_dirSpkBinMetaDataTmp.polarPosition.azimuth = 0.;
                m_dirSpkBinMetaDataTmp.polarPosition.elevation = -30.;
            }

            // Get the gain vector to be applied to the DirectSpeaker channel
            m_directSpeakerGainCalc->calculateGains(m_dirSpkBinMetaDataTmp, m_directSpeakerGains);
        }
        else
        {
            // Get the gain vector to be applied to the DirectSpeaker channel
            m_directSpeakerGainCalc->calculateGains(metadata, m_directSpeakerGains);
        }

        // Apply the metadata gain to the gain vector
        for (auto& g : m_directSpeakerGains)
            g *= metadata.gain;

        // Map from the track index to the corresponding panner index
        int nObjectInd = GetMatchingIndex(m_pannerTrackInd, metadata.trackInd, TypeDefinition::DirectSpeakers);

        if (nObjectInd == -1) // this track was not declared at construction. Stopping here.
        {
            std::cerr << "Renderer Error: Expected a track index that was declared an DirectSpeaker in construction. Input will not be rendered." << std::endl;
            return;
        }
        int iDirSpk = m_channelToDirectSpeakerMap[nObjectInd];

        m_directSpeakerGainInterp[iDirSpk].SetGainVector(m_directSpeakerGains, m_gainInterpTime);
        m_directSpeakerGainInterp[iDirSpk].ProcessAccumul(pDirSpkIn, m_speakerOut, nSamples, nOffset);
    }

    void Renderer::AddBinaural(float** pBinIn, unsigned int nSamples, unsigned int nOffset)
    {
        if (m_RenderLayout == OutputLayout::Binaural)
        {
            // Add the binaural signals directly to the output buffer with no processing
            for (unsigned int iEar = 0; iEar < 2; ++iEar)
                for (unsigned int iSample = 0; iSample < nSamples; ++iSample)
                    m_binauralOut[iEar][iSample + nOffset] += pBinIn[iEar][iSample];
        }
    }

    void Renderer::GetRenderedAudio(float** pRender, unsigned int nSamples)
    {
        // Apply diffuseness filters and compensation delay
        m_decorrelate.Process(m_speakerOutDirect, m_speakerOutDiffuse, nSamples);

        if (m_RenderLayout == OutputLayout::Binaural)
        {
            // Add the signals that have already been routed to the speaker layout to the output buffer
            for (unsigned int iSpk = 0; iSpk < m_nChannelsToRender; ++iSpk)
                for (unsigned int iSample = 0; iSample < nSamples; ++iSample)
                    m_virtualSpeakerOut[iSpk][iSample] += m_speakerOut[iSpk][iSample] + m_speakerOutDirect[iSpk][iSample] + m_speakerOutDiffuse[iSpk][iSample];

            // Encode speaker signals to HOA
            for (size_t iSpk = 0; iSpk < m_outputLayout.getNumChannels(); ++iSpk)
                m_hoaEncoders[iSpk].ProcessAccumul(m_virtualSpeakerOut[iSpk], nSamples, &m_hoaAudioOut);

            // Rotate the sound field to match the head orientation
            m_hoaRotate.Process(&m_hoaAudioOut, nSamples);

            // Decode HOA to binaural
            m_hoaBinaural.Process(&m_hoaAudioOut, pRender);

            // Add the binaural signals to the output
            for (unsigned int iEar = 0; iEar < 2; ++iEar)
                for (unsigned int iSample = 0; iSample < nSamples; ++iSample)
                    pRender[iEar][iSample] += m_binauralOut[iEar][iSample];

            // Clear the data in the binaural buffer for the next frame
            ClearBinauralBuffer();
            // Clear the data in the virtual speaker buffers for the next frame
            ClearVirtualSpeakerBuffer();
        }
        else
        {
            // Decode the HOA stream to the output buffer
            m_hoaDecoder.Process(&m_hoaAudioOut, nSamples, pRender);

            // Add the signals that have already been routed to the speaker layout to the output buffer
            for (unsigned int iSpk = 0; iSpk < m_nChannelsToRender; ++iSpk)
                for (unsigned int iSample = 0; iSample < nSamples; ++iSample)
                    pRender[iSpk][iSample] += m_speakerOut[iSpk][iSample] + m_speakerOutDirect[iSpk][iSample] + m_speakerOutDiffuse[iSpk][iSample];
        }

        // Apply the output gain
        for (unsigned int iOut = 0; iOut < m_nChannelsToOutput; ++iOut)
        {
            float* ppOut[1] = { pRender[iOut] };
            m_outGainInterp[iOut].Process(pRender[iOut], ppOut, nSamples, 0);
        }

        // Clear the HOA data for the next frame
        ClearHoaBuffer();
        // Clear the output buffer
        ClearOutputBuffer();
        // Clear the Object direct signal data
        ClearObjectDirectBuffer();
        // Clear the data in the diffuse buffers
        ClearObjectDiffuseBuffer();
    }

    void Renderer::ClearHoaBuffer()
    {
        m_hoaAudioOut.Reset();
    }

    void Renderer::ClearOutputBuffer()
    {
        for (unsigned int iCh = 0; iCh < m_nChannelsToRender; ++iCh)
            for (unsigned int iSamp = 0; iSamp < m_nSamples; ++iSamp)
                m_speakerOut[iCh][iSamp] = 0.f;
    }

    void Renderer::ClearObjectDirectBuffer()
    {
        for (unsigned int iCh = 0; iCh < m_nChannelsToRender; ++iCh)
            for (unsigned int iSamp = 0; iSamp < m_nSamples; ++iSamp)
                m_speakerOutDirect[iCh][iSamp] = 0.f;
    }

    void Renderer::ClearObjectDiffuseBuffer()
    {
        for (unsigned int iCh = 0; iCh < m_nChannelsToRender; ++iCh)
            for (unsigned int iSamp = 0; iSamp < m_nSamples; ++iSamp)
                m_speakerOutDiffuse[iCh][iSamp] = 0.f;
    }

    void Renderer::ClearBinauralBuffer()
    {
        for (unsigned int iCh = 0; iCh < 2; ++iCh)
            for (unsigned int iSamp = 0; iSamp < m_nSamples; ++iSamp)
                m_binauralOut[iCh][iSamp] = 0.f;
    }

    void Renderer::ClearVirtualSpeakerBuffer()
    {
        for (unsigned int iSpk = 0; iSpk < m_nChannelsToRender; ++iSpk)
            for (unsigned int iSample = 0; iSample < m_nSamples; ++iSample)
                m_virtualSpeakerOut[iSpk][iSample] = 0.f;
    }

    int Renderer::GetMatchingIndex(const std::vector<std::pair<unsigned int, TypeDefinition>>& vector, unsigned int nElement, TypeDefinition trackType)
    {
        // Map from the track index to the corresponding panner index
        int nInd = 0;
        for (unsigned int i = 0; i < vector.size(); i++)
            if (vector[i].first == nElement && vector[i].second == trackType) {
                nInd = (int)i;
                return nInd;
            }

        return -1;
    }

    void Renderer::AllocateBuffers(float**& buffers, unsigned nCh, unsigned nSamples)
    {
        DeallocateBuffers(buffers, nCh);
        buffers = new float* [nCh];
        for (unsigned int iCh = 0; iCh < nCh; ++iCh)
        {
            buffers[iCh] = new float[nSamples];
            memset(buffers[iCh], 0, m_nSamples * sizeof(float));
        }
    }

    void Renderer::DeallocateBuffers(float**& buffers, unsigned nCh)
    {
        if (buffers)
        {
            for (unsigned int iCh = 0; iCh < nCh; ++iCh)
                if (buffers[iCh])
                    delete[] buffers[iCh];
            delete[] buffers;
            buffers = nullptr;
        }
    }

} // namespace spaudio

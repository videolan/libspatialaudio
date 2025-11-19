#include "RendererMetadata.h"
#undef NDEBUG
#include <cassert>
#include <iostream>
#include <iomanip>
#include <array>

#include "Renderer.h"
#include "LoudspeakerLayouts.h"

int main() {
    constexpr unsigned nSamples = 1024;
    constexpr unsigned order = 1;
    constexpr unsigned sampleRate = 48000;
    constexpr spaudio::OutputLayout outputLayout = spaudio::OutputLayout::Binaural;
    constexpr unsigned outputChannels = 2;

    spaudio::StreamInformation streamInfo;
    // 1 channel object (mono)
    // 2 channels speaker (stereo)
    streamInfo.nChannels = 3;
    streamInfo.typeDefinition = {
        spaudio::TypeDefinition::Objects,
        spaudio::TypeDefinition::DirectSpeakers,
        spaudio::TypeDefinition::DirectSpeakers,
    };

    spaudio::Renderer renderer;
    renderer.Configure(outputLayout, order, sampleRate, nSamples, streamInfo, "", true);

    std::array<float, nSamples> impulse;
    impulse.fill(0.f);
    impulse[0] = 1.f;

    float** ldspkOut = new float* [outputChannels];
    for (int iLdspk = 0; iLdspk < outputChannels; ++iLdspk)
        ldspkOut[iLdspk] = new float[nSamples];

    spaudio::ObjectMetadata objMetadata;
    objMetadata.blockLength = nSamples;
    objMetadata.trackInd = 0;
    objMetadata.position = spaudio::PolarPosition<double>{90., 0.f, 1.f};
    objMetadata.jumpPosition.flag = true;
    renderer.AddObject(impulse.data(), nSamples, objMetadata);

    spaudio::DirectSpeakerMetadata speakerMetadata;
    speakerMetadata.trackInd = 1;
    speakerMetadata.polarPosition = spaudio::DirectSpeakerPolarPosition{ 90., 0.f, 1.f };
    speakerMetadata.speakerLabel = "M+090";
    renderer.AddDirectSpeaker(impulse.data(), nSamples, speakerMetadata);

    speakerMetadata.trackInd = 2;
    speakerMetadata.polarPosition = spaudio::DirectSpeakerPolarPosition{ -90., 0.f, 1.f };
    speakerMetadata.speakerLabel = "M-090";
    renderer.AddDirectSpeaker(impulse.data(), nSamples, speakerMetadata);

    renderer.GetRenderedAudio(ldspkOut, nSamples);
    for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp) {
        std::cout << std::setw(4) << iSamp << ": ";
        for (unsigned iLdspk = 0; iLdspk < outputChannels; ++iLdspk)
        {
            std::cout << std::setw(12) << std::fixed <<
            ldspkOut[iLdspk][iSamp] << std::setw(0) << ", ";
        }
        std::cout << std::endl;
    }

    for (int iLdspk = 0; iLdspk < outputChannels; ++iLdspk)
        delete[] ldspkOut[iLdspk];
    delete[] ldspkOut;
}

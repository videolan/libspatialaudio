#undef NDEBUG
#include <cassert>
#include <iostream>
#include <iomanip>
#include <array>

#include "AmbisonicEncoder.h"
#include "AmbisonicDecoder.h"

int main() {
    constexpr unsigned nSamples = 1;
	unsigned order = 1;
	bool b3D = true;
	unsigned sampleRate = 48000;
	auto layout = spaudio::Amblib_SpeakerSetUps::kAmblib_71;

	spaudio::AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, b3D, sampleRate, 0.f);
	assert(success);

	spaudio::AmbisonicDecoder ambiDec;
	success = ambiDec.Configure(order, b3D, nSamples, sampleRate, layout);
	assert(success);
	assert(ambiDec.GetPresetLoaded());

	unsigned nLdspk = ambiDec.GetSpeakerCount();

	spaudio::BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::array<float, nSamples> impulse;
	impulse.fill(0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		ambiEnc.SetPosition({ spaudio::DegreesToRadians(az), 0.f, 1.f });
		ambiEnc.Refresh();
		ambiEnc.Process(impulse.data(), nSamples, &inputSignal);

		ambiDec.Process(&inputSignal, nSamples, ldspkOut);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp) {
		    std::cout << std::setw(4) << iSamp << ": ";
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
			{
			    std::cout << std::setw(12) << std::fixed <<
			    ldspkOut[iLdspk][iSamp] << std::setw(0) << ", ";
			}
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete[] ldspkOut[iLdspk];
	delete[] ldspkOut;
}

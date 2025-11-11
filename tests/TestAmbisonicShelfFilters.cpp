#undef NDEBUG
#include <cassert>
#include <iostream>
#include <iomanip>
#include <array>

#include "AmbisonicShelfFilters.h"

int main() {
    spaudio::AmbisonicShelfFilters shelfFilters;

    spaudio::BFormat outputSignal;
    constexpr size_t nSamples = 128;
    constexpr unsigned order = 3;
    constexpr bool b3D = true;

    outputSignal.Configure(order, b3D, nSamples);
    outputSignal.Reset();

    bool ret = shelfFilters.Configure(order, b3D, nSamples, 0);
    assert(ret);

    std::array<float, nSamples> impulse;
    std::vector<std::vector<float>> output(outputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));

    impulse.fill(0.f);
    impulse[0] = 1.f;

    for (unsigned i = 0; i < outputSignal.GetChannelCount(); ++i)
        outputSignal.InsertStream(impulse.data(), i, nSamples);

    shelfFilters.Process(&outputSignal, nSamples);

    for (unsigned i = 0; i < outputSignal.GetChannelCount(); ++i)
        outputSignal.ExtractStream(output[i].data(), i, nSamples);

   	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp) {
		std::cout << std::setw(4) << iSamp << ": ";
		for (unsigned iCh = 0; iCh < outputSignal.GetChannelCount(); ++iCh)
		{
			std::cout << std::setw(12) << std::fixed <<
			    output[iCh][iSamp] << std::setw(0) << ", ";
		}
		std::cout << std::endl;
	}
}

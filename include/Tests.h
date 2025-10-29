/*############################################################################*/
/*#                                                                          #*/
/*#  Some tests for the ADM classes											 #*/
/*#                                                                          #*/
/*#  Filename:      Tests.h		                                             #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          06/11/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#pragma once

#include <assert.h>
#include <iostream>
#include <iomanip>
#include "LoudspeakerLayouts.h"
#include "dsp/LinkwitzRileyIIR.h"
#include "ObjectPanner.h"
#include "Renderer.h"
#include "adm/AllocentricExtent.h"

using namespace spaudio;

/**
	Get the gain vector for a signal routed directed to a specified speaker in the layout
*/
static inline std::vector<double> getDirectGain(std::string speakerName, Layout layout)
{
	std::vector<double> g(layout.getNumChannels(), 0.);
	if (layout.getMatchingChannelIndex(speakerName) >= 0)
		g[layout.getMatchingChannelIndex(speakerName)] = 1.;

	return g;
}

/**
	Insert LFE channels in a vector of gains from the point source panner
*/
static inline std::vector<double> insertLFE(std::vector<double> a, Layout layout)
{
	std::vector<double> g(layout.getNumChannels());
	int iCount = 0;
	for (int i = 0; i < layout.getNumChannels(); ++i)
		if (!layout.getChannel(i).getIsLfe())
			g[i] = a[iCount++];

	return g;
}

static inline bool compareGainVectors(std::vector<double> a, std::vector<double> b)
{
	assert(a.size() == b.size());
	return std::abs(norm(vecSubtract(a, b))) < 1e-5;
}

/**
	Test the functioning of the DirectSpeaker gain calculator with different conditions.
	Returns true if all return the expected results
*/
static inline bool testDirectSpeakers()
{
	DirectSpeakerMetadata metadata;
	metadata.audioPackFormatID = std::string("AP_00010004");

	Layout layout = Layout::getMatchingLayout(ituPackNames.find("AP_00010004")->second);
	// Need to set the reproduction screen in the layout for screen edge locking to work
	Screen screen;
	double screenWidth = 20.;
	screen.widthAzimuth = screenWidth;
	layout.setReproductionScreen(screen);
	spaudio::adm::DirectSpeakersGainCalc gainCalc(layout);
	PointSourcePannerGainCalc psp(layout);
	size_t nCh = layout.getNumChannels();
	size_t nChNoLfe = Layout::getLayoutWithoutLFE(layout).getNumChannels();

	std::vector<std::string> speakerNamePrefixes = { "", "urn:itu:bs:2051:0:speaker:", "urn:itu:bs:2051:1:speaker:" };

	for (auto& p : speakerNamePrefixes)
	{
		// Test that the signal is routed to the correct loudspeaker based on the name of a speaker in the array
		std::string speakerLabel = "M+030";
		metadata.speakerLabel = p + speakerLabel;
		std::vector<double> gName(nCh);
		gainCalc.calculateGains(metadata, gName);
		auto gNameTest = getDirectGain(speakerLabel, layout);
		bool bName = gName == gNameTest;
		assert(bName);

		// Test that the signal is routed to the correct loudspeakers for a speaker not in the array
		// This should use the mapping rules
		speakerLabel = "U+180";
		metadata.speakerLabel = p + speakerLabel;
		std::vector<double> gMapping(nCh);
		gainCalc.calculateGains(metadata, gMapping);
		auto gMappingTest = std::vector<double>{ 0.,0.,0.,0.,1. / std::sqrt(2.),1. / std::sqrt(2.),0.,0. };
		bool bMapping = gMapping == gMappingTest;
		assert(bMapping);
	}

	// Test that the signal is routed to the LFE if the low-pass frequency has been set < 200 Hz
	metadata.channelFrequency.lowPass = 100.;
	metadata.speakerLabel = "";
	std::vector<double> gFreq(nCh);
	gainCalc.calculateGains(metadata, gFreq);
	auto gFreqTest = getDirectGain("LFE1", layout);
	bool bFreq = gFreq == gFreqTest;
	assert(bFreq);

	// Test that the LFE labels route to the LFE channel
	metadata.channelFrequency.lowPass.reset();// Remove low frequency data and test routing based on LFE names
	std::vector<std::string> lfeNames = { "LFE","LFEL","LFER","LFE1","LFE2" };
	for (auto& lfe : lfeNames)
	{
		metadata.speakerLabel = lfe;
		std::vector<double> gLFE(nCh);
		gainCalc.calculateGains(metadata, gLFE);
		auto gLFETest = getDirectGain("LFE1", layout);
		bool bLFE = gFreq == gLFETest;
		assert(bLFE);
	}

	// Test no pack format set but with a speaker label matching one in the layout
	metadata.audioPackFormatID.reset();
	std::string speakerLabel = "M-030";
	metadata.speakerLabel = speakerLabel;
	std::vector<double> gNoPackMatch(nCh);
	gainCalc.calculateGains(metadata, gNoPackMatch);
	auto gNoPackMatchTest = getDirectGain(speakerLabel, layout);
	bool bNoPackMatch = gNoPackMatch == gNoPackMatchTest;
	assert(bNoPackMatch);

	// Test screen edge locking
	speakerLabel = "";
	metadata.speakerLabel = speakerLabel;
	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::LEFT;
	std::vector<double> gEdgeLock(nCh);
	gainCalc.calculateGains(metadata, gEdgeLock);
	std::vector<double> gEdgeLockTestTmp(nChNoLfe);
	psp.CalculateGains(PolarPosition<double>{ screenWidth / 2.,0.,1. }, gEdgeLockTestTmp);
	auto gEdgeLockTest = insertLFE(gEdgeLockTestTmp, layout);
	bool bEdgeLock = compareGainVectors(gEdgeLock, gEdgeLockTest);
	assert(bEdgeLock);

	// Test with bounds
	// Should find the M+030 speaker within the bounds set
	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::NO_HOR;
	metadata.polarPosition.azimuth = 28.;
	metadata.polarPosition.elevation = 5.;
	metadata.polarPosition.distance = 1.;
	metadata.polarPosition.bounds = PolarBounds{ 25.,35.,-10.,10.,0.9,1.1 };
	std::vector<double> gBounds(nCh);
	gainCalc.calculateGains(metadata, gBounds);
	auto gBoundsTest = getDirectGain("M+030", layout);
	bool bBounds = compareGainVectors(gBounds, gBoundsTest);
	assert(bBounds);

	// Should not find any speaker within bounds so fall back to PSP
	metadata.polarPosition.azimuth = 28.;
	metadata.polarPosition.elevation = 5.;
	metadata.polarPosition.distance = 1.;
	metadata.polarPosition.bounds = PolarBounds{ 27.,29.,-10.,10.,0.9,1.1 };
	std::vector<double> gNoBounds(nCh);
	gainCalc.calculateGains(metadata, gNoBounds);
	std::vector<double> gNoBoundsTestTmp(nChNoLfe);
	psp.CalculateGains(PolarPosition<double>{ metadata.polarPosition.azimuth,metadata.polarPosition.elevation,metadata.polarPosition.distance }, gNoBoundsTestTmp);
	auto gNoBoundsTest = insertLFE(gNoBoundsTestTmp, layout);
	bool bNoBounds = compareGainVectors(gNoBounds, gNoBoundsTest);
	assert(bNoBounds);

	// Test the bounds if 2 speakers are within the bounds. Should go to the closer of the two
	metadata.polarPosition.azimuth = 16.;
	metadata.polarPosition.elevation = 0.;
	metadata.polarPosition.distance = 1.;
	metadata.polarPosition.bounds = PolarBounds{ 0.,30.,0.,0.,1.,1. };
	std::vector<double> gTwoBounds(nCh);
	gainCalc.calculateGains(metadata, gTwoBounds);
	auto gTwoBoundsTest = getDirectGain("M+030", layout);
	bool bTwoBounds = compareGainVectors(gTwoBounds, gTwoBoundsTest);
	assert(bTwoBounds);

	// Test fallback panning with the PSP
	metadata.polarPosition = { 10., 5., 1. };
	std::vector<double> gFallback(nCh);
	gainCalc.calculateGains(metadata, gFallback);
	std::vector<double> gFallbackTmp(nChNoLfe);
	psp.CalculateGains(PolarPosition<double>{ metadata.polarPosition.azimuth,metadata.polarPosition.elevation,metadata.polarPosition.distance }, gFallbackTmp);
	auto gFallbackTest = insertLFE(gFallbackTmp, layout);
	bool bFallback = compareGainVectors(gFallback, gFallbackTest);
	assert(bFallback);

	return true;
}

/**
	Test the point source panner around the horizontal across all layouts that have been defined, except HOA and stereo.
*/
static inline bool testPointSourcePanner()
{
	// Loop through all layouts
	for (auto& layout : Layout::getSpeakerLayouts())
	{
		auto layoutNoLFE = Layout::getLayoutWithoutLFE(layout);
		PointSourcePannerGainCalc psp(layout);

		auto nCh = layout.getNumChannels();
		auto nChNoLfe = layoutNoLFE.getNumChannels();

		for (double az = 0.; az <= 360.; az += 5.)
		{
			auto position = PolarToCartesian(PolarPosition<double>{ az, 0., 1. });
			std::vector<double> gains(nChNoLfe);
			psp.CalculateGains(position, gains);
			auto gainSum = std::accumulate(gains.begin(), gains.end(), 0.);
			auto numCh = psp.getNumChannels();

			// Calculate the velocity vector based on the loudspeaker gains and directions
			CartesianPosition<double> velVec;
			velVec.x = 0.;
			velVec.y = 0.;
			velVec.z = 0.;
			for (unsigned int i = 0; i < numCh; ++i)
			{
				CartesianPosition<double> speakerDirections = PolarToCartesian(layoutNoLFE.getChannel(i).getPolarPosition());
				velVec = velVec + CartesianPosition<double>{ gains[i] * speakerDirections.x, gains[i] * speakerDirections.y,gains[i] * speakerDirections.z, };
			}
			velVec.x /= gainSum;
			velVec.y /= gainSum;
			velVec.z /= gainSum;

			// Convert to a unit vector since for VBAP-type panning the velocity vector usually has a magnitude less than 1 except when on a single speakers
			auto velVecNorm = norm(velVec);
			velVec.x /= velVecNorm;
			velVec.y /= velVecNorm;
			velVec.z /= velVecNorm;

			// Check the direction matches with the input position
			if ((layout.getLayoutName() != "0+2+0" && layout.getLayoutName() != "2+3+0") || std::abs(az) <= 30. || az >= 330.)
				assert(norm(position - velVec) < 1e-5);
		}
	}

	return true;
}

/**
	Test the gain calculator with different metadata conditions
*/
static inline bool testGainCalculator()
{
	ObjectMetadata metadata;
	metadata.referenceScreen = Screen();

	Layout layout = Layout::getMatchingLayout(ituPackNames.find("AP_00010004")->second);
	// Need to set the reproduction screen in the layout for screen scaling and screen edge locking to work
	Screen screen;
	double screenWidth = 20.;
	screen.widthAzimuth = screenWidth;
	layout.setReproductionScreen(screen);
	spaudio::adm::ObjectGainCalculator gainCalc(layout);
	PointSourcePannerGainCalc psp(layout);
	auto nCh = layout.getNumChannels();

	std::vector<double> directGains(nCh), diffuseGains(nCh);

	// Test the panning without any other metadata parameters set
	auto& metadataPolarPos = metadata.position.polarPosition();
	metadataPolarPos.azimuth = 28.;
	metadataPolarPos.elevation = 5.;
	metadataPolarPos.distance = 1.;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	std::vector<double> directGainsTest(nCh);
	psp.CalculateGains(metadataPolarPos, directGainsTest);
	assert(compareGainVectors(directGains, insertLFE(directGainsTest, layout)));

	// Test the screen scaling
	// First test a source on the edge of the screen moves
	metadata.screenRef = true;
	metadataPolarPos.azimuth = Screen().widthAzimuth / 2.;
	metadataPolarPos.elevation = 0.;
	metadataPolarPos.distance = 1.;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	psp.CalculateGains(PolarPosition<double>{ screenWidth / 2.,0.,1. }, directGainsTest);
	assert(compareGainVectors(directGains, insertLFE(directGainsTest, layout)));

	// Test the case where the reference screen was not in the centre but the reproduction one is
	metadata.referenceScreen.centrePolarPosition = { 30.,0.,1. };
	metadata.referenceScreen.widthAzimuth = 60.;
	metadataPolarPos = { 30.,0.,1. };
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+000", layout)));

	// Test the case where the reference screen was in the centre but the reproduction screen is not
	metadata.referenceScreen = Screen(); // default reproduction screens
	metadataPolarPos = { 0.,0.,1. };
	Layout layoutScreenScaling = layout;
	screen = Screen();
	screen.widthAzimuth = 60.;
	screen.centrePolarPosition = { 30.,0.,1. };
	layoutScreenScaling.setReproductionScreen(screen);
	spaudio::adm::ObjectGainCalculator gainCalcScrnScale(layoutScreenScaling);
	gainCalcScrnScale.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+030", layout)));

	// Switch off screen scaling
	metadata.screenRef = false;

	// Test screen edge lock - left
	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::LEFT;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	std::vector<double> gainsEdge(nCh);
	psp.CalculateGains(PolarPosition<double>{ screenWidth / 2.,0.,1. }, gainsEdge);
	assert(compareGainVectors(directGains, insertLFE(gainsEdge, layout)));

	// Test screen edge lock - right
	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::RIGHT;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	psp.CalculateGains(PolarPosition<double>{ -screenWidth / 2.,0.,1. }, gainsEdge);
	assert(compareGainVectors(directGains, insertLFE(gainsEdge, layout)));

	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::NO_HOR;

	// Test screen edge lock - top
	metadata.screenEdgeLock.vertical = ScreenEdgeLock::TOP;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	double screenTopEdge = RAD2DEG * std::atan(std::tan(DEG2RAD * 0.5 * screenWidth) / layout.getReproductionScreen()->aspectRatio);
	psp.CalculateGains(PolarPosition<double>{ 0.,screenTopEdge,1. }, gainsEdge);
	assert(compareGainVectors(directGains, insertLFE(gainsEdge, layout)));

	// Test screen edge lock - bottom
	metadata.screenEdgeLock.vertical = ScreenEdgeLock::BOTTOM;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	psp.CalculateGains(PolarPosition<double>{ 0.,-screenTopEdge,1. }, gainsEdge);
	assert(compareGainVectors(directGains, insertLFE(gainsEdge, layout)));


	// Switch off screen edge lock
	metadata.screenEdgeLock.horizontal = ScreenEdgeLock::NO_HOR;
	metadata.screenEdgeLock.vertical = ScreenEdgeLock::NO_VERT;


	// Test channel lock - source on speakers
	metadataPolarPos = { 30.,0.,1. };
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);

	// Test channel lock - source closer to one speaker than another (left)
	metadataPolarPos = { 14,0.,1. };
	metadata.channelLock = ChannelLock();
	metadata.channelLock->maxDistance = 0.5;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+000", layout)));
	metadataPolarPos = { 16,0.,1. };
	metadata.channelLock->maxDistance = 0.5;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+030", layout)));

	// Test channel lock - source closer to one speaker than another (right)
	metadataPolarPos = { -14,0.,1. };
	metadata.channelLock->maxDistance = 0.5;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+000", layout)));
	metadataPolarPos = { -16,0.,1. };
	metadata.channelLock->maxDistance = 0.5;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M-030", layout)));

	// Test channel lock - source equidistant between two speakers and in range
	metadataPolarPos = { 15.,0.,1. };
	metadata.channelLock->maxDistance = 1.;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M+000", layout)));

	// Test channel lock - source equidistant between two speakers with the same abs(azimuth) and in range
	metadataPolarPos = { 180.,0.,1. };
	metadata.channelLock->maxDistance = 2.;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("M-110", layout)));

	// Test channel lock - source equidistant between two speakers but not in range
	metadataPolarPos = { 180.,0.,1. };
	metadata.channelLock->maxDistance = 0.05;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, { 0.,0.,0.,0.,std::sqrt(0.5),std::sqrt(0.5),0.,0. }));

	// Test channel lock - closer to upper layer speaker
	metadataPolarPos = { 30.,16.,1. };
	metadata.channelLock->maxDistance = 1.;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	assert(compareGainVectors(directGains, getDirectGain("U+030", layout)));


	// Remove channel locking
	metadata.channelLock->maxDistance = 0.;

	// Test the diffuseness parameter
	metadata.diffuse = 0.25;
	gainCalc.CalculateGains(metadata, directGains, diffuseGains);
	std::vector<double> pspGains(nCh);
	psp.CalculateGains(metadataPolarPos, pspGains);
	auto pspDirect = pspGains;
	auto pspDiffuse = pspGains;
	for (int i = 0; i < (int)pspGains.size(); ++i)
	{
		pspDirect[i] *= std::sqrt(1. - metadata.diffuse);
		pspDiffuse[i] *= std::sqrt(metadata.diffuse);
	}
	assert(compareGainVectors(directGains, insertLFE(pspDirect, layout)));
	assert(compareGainVectors(diffuseGains, insertLFE(pspDiffuse, layout)));

	return true;
}

/** Test the Linkwitz-Riley filter sums to flat magnitude
*/
void testLinkwitzRileyFilter()
{
	LinkwitzRileyIIR lrIIR;

	unsigned int sampleRate = 48000;
	const unsigned int nSamples = 256;
	const int nCh = 2;
	// Set up the signal for 2 seconds worth
	float** pIn = new float* [nCh];
	float** pOutLP = new float* [nCh];
	float** pOutHP = new float* [nCh];
	for (int iCh = 0; iCh < nCh; ++iCh)
	{
		pIn[iCh] = new float[nSamples];
		pOutLP[iCh] = new float[nSamples];
		pOutHP[iCh] = new float[nSamples];
		for (int iSample = 0; iSample < nSamples; ++iSample)
		{
			pIn[iCh][iSample] = iSample == 0 ? 1.f : 0.f;
			pOutLP[iCh][iSample] = 0.f;
			pOutHP[iCh][iSample] = 0.f;
		}
	}

	lrIIR.Configure(nCh, sampleRate, 500.f);
	lrIIR.Process(pIn, pOutLP, pOutHP, nSamples);

	float** pSum = new float* [nCh];
	for (int iCh = 0; iCh < nCh; ++iCh)
	{
		pSum[iCh] = new float[nSamples];
		for (int iSample = 0; iSample < nSamples; ++iSample)
		{
			pSum[iCh][iSample] = pOutLP[iCh][iSample] + pOutHP[iCh][iSample];
		}
	}

	// Check the IR
	kiss_fftr_cfg st = kiss_fftr_alloc(nSamples, 0, 0, 0);
	kiss_fft_cpx* out = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * (nSamples / 2 + 1));
	kiss_fftr(st, pSum[0], (kiss_fft_cpx*)out);
	for (int iSample = 0; iSample < nSamples / 2 + 1; ++iSample)
	{
		float mag = std::sqrt(out[iSample].r * out[iSample].r + out[iSample].i * out[iSample].i);
		assert(std::abs(20.f * std::log10(mag)) < 1e-3); // Less that 0.001dB deviation
	}
	free(out);
	free(st);

	// Deallocate
	for (int iCh = 0; iCh < nCh; ++iCh)
	{
		delete[] pIn[iCh];
		delete[] pOutLP[iCh];
		delete[] pOutHP[iCh];
		delete[] pSum[iCh];
	}
	delete[] pIn;
	delete[] pOutLP;
	delete[] pOutHP;
	delete[] pSum;
}

/** Test the psychoacoustic optimisation filters
*/
void testOptimFilters()
{
	AmbisonicOptimFilters optFilters;

	BFormat inputSignal;
	unsigned nSamples = 128;
	unsigned order = 3;
	bool b3D = true;
	unsigned sampleRate = 48000;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();

	optFilters.Configure(order, b3D, nSamples, sampleRate);

	std::vector<float> impulse(nSamples, 0.f);
	std::vector<std::vector<float>> output(inputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));
	impulse[0] = 1.f;
	for (unsigned i = 0; i < inputSignal.GetChannelCount(); ++i)
		inputSignal.InsertStream(impulse.data(), i, nSamples);

	optFilters.Process(&inputSignal, nSamples);

	for (unsigned i = 0; i < inputSignal.GetChannelCount(); ++i)
		inputSignal.ExtractStream(output[i].data(), i, nSamples);
}

/** Test the binaural decoder
*/
void testBinauralDecoder()
{
	unsigned nSamples = 128;
	unsigned order = 1;
	bool b3D = true;
	unsigned sampleRate = 48000;
	unsigned tailLength = 0;

	AmbisonicBinauralizer ambiBin;
	bool success = ambiBin.Configure(order, b3D, sampleRate, nSamples, tailLength);
	assert(success);

	BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::vector<float> impulse(nSamples, 0.f);
	std::vector<std::vector<float>> output(inputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));
	impulse[0] = 1.f;
	// Encoded at 90deg
	inputSignal.InsertStream(impulse.data(), 0, nSamples);
	inputSignal.InsertStream(impulse.data(), 1, nSamples);

	float** binOut = new float*[2];
	for (int iEar = 0; iEar < 2; ++iEar)
		binOut[iEar] = new float[nSamples];

	ambiBin.Process(&inputSignal, binOut, nSamples);

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		std::cout << binOut[0][iSamp] << ", " << binOut[1][iSamp] << std::endl;

	for (int iEar = 0; iEar < 2; ++iEar)
		delete binOut[iEar];
	delete[] binOut;
}

/** Test sound field rotation */
void testRotation()
{
	unsigned nSamples = 128;
	unsigned order = 3;
	bool b3D = true;
	unsigned sampleRate = 48000;
	float fadeTime = 0.5f * 1000.f * (float)nSamples / (float)sampleRate;

	AmbisonicRotator ambiRot;
	ambiRot.Configure(order, b3D, nSamples, sampleRate, fadeTime);

	BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::vector<float> ones(nSamples, 1.f);
	// Encoded the all-ones signal to HOA
	PolarPosition<float> srcPos;
	srcPos.azimuth = 0.25f * 3.14159f;
	srcPos.elevation = 0.25f * 3.14159f;
	srcPos.distance = 1.f;
	AmbisonicEncoder ambiEnc;
	ambiEnc.Configure(order, b3D, sampleRate, 0);
	ambiEnc.SetPosition(srcPos);
	ambiEnc.Refresh();
	ambiEnc.Process(ones.data(), nSamples, &inputSignal);
	BFormat originalSig;
	originalSig.Configure(order, b3D, nSamples);
	originalSig = inputSignal;

	// Set the orientation just before processing to ensure cross-fade is triggered
	RotationOrientation ori = { 0.33f * 3.14159f, 0.33f * 3.14159f, 0.33f * 3.14159f };
	ambiRot.SetOrientation(ori);

	// Process using both processors
	ambiRot.Process(&inputSignal, nSamples);

	std::vector<std::vector<float>> outStream(inputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f))
		, origSig(inputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));

	for (unsigned iCh = 0; iCh < inputSignal.GetChannelCount(); ++iCh)
	{
		inputSignal.ExtractStream(outStream[iCh].data(), iCh, nSamples);
		originalSig.ExtractStream(origSig[iCh].data(), iCh, nSamples);
	}

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
	{
		std::cout << iSamp << ": ";
		for (unsigned iCh = 0; iCh < inputSignal.GetChannelCount(); ++iCh)
		{
			std::cout << origSig[iCh][iSamp] << "/" << outStream[iCh][iSamp] << ", ";
		}
		std::cout << std::endl;
	}

}

/** Test the gain interpolation of AmbisonicEncoder
*/
void testHoaEncoding()
{
	unsigned nSamples = 128;
	unsigned order = 1;
	bool b3D = true;
	unsigned sampleRate = 48000;

	AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, b3D, sampleRate, (float)nSamples / (float)sampleRate * 1000.f);
	assert(success);
	ambiEnc.SetPosition({ 0.f,0.f,1.f });
	ambiEnc.Reset();

	BFormat outputSignal;
	outputSignal.Configure(order, b3D, nSamples);
	outputSignal.Reset();
	std::vector<float> ones(nSamples, 1.f);

	std::vector<std::vector<float>> outStream(outputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));

	ambiEnc.SetPosition({ DegreesToRadians(90.f),0.f,1. });
	ambiEnc.Process(ones.data(), nSamples, &outputSignal);

	for (unsigned iCh = 0; iCh < outputSignal.GetChannelCount(); ++iCh)
		outputSignal.ExtractStream(outStream[iCh].data(), iCh, nSamples);

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
	{
		std::cout << iSamp << ": ";
		for (unsigned iCh = 0; iCh < outputSignal.GetChannelCount(); ++iCh)
		{
			std::cout << outStream[iCh][iSamp] << ", ";
		}
		std::cout << std::endl;
	}
}

/** Test the loudspeaker decoder presets
*/
void testDecoderPresets()
{
	unsigned nSamples = 1;
	unsigned order = 1;
	bool b3D = true;
	unsigned sampleRate = 48000;
	auto layout = Amblib_SpeakerSetUps::kAmblib_71;

	AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, b3D, sampleRate, 0.f);
	assert(success);

	AmbisonicDecoder ambiDec;
	success = ambiDec.Configure(order, b3D, nSamples, sampleRate, layout);
	assert(success);

	unsigned nLdspk = ambiDec.GetSpeakerCount();

	BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		ambiEnc.SetPosition({ DegreesToRadians(az),0.f,1. });
		ambiEnc.Refresh();
		ambiEnc.Process(impulse.data(), nSamples, &inputSignal);

		ambiDec.Process(&inputSignal, nSamples, ldspkOut);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		{
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
				std::cout << ldspkOut[iLdspk][iSamp] << ", ";
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the routing of ADM HOA decoding to presets
*/
void testAdmHoaDecodingRouting()
{
	auto outputLayout = spaudio::OutputLayout::FivePointOne;
	unsigned nLdspk = 6;
	unsigned order = 1;
	unsigned sampleRate = 48000;
	unsigned nSamples = 1;
	StreamInformation channelInfo;
	channelInfo.nChannels = OrderToComponents(order, true);
	channelInfo.typeDefinition.resize(channelInfo.nChannels, TypeDefinition::HOA);

	HoaMetadata metadata;
	metadata.normalization = "SN3D";
	metadata.trackInds.resize(channelInfo.nChannels);

	for (unsigned i = 0; i < channelInfo.nChannels; ++i)
		metadata.trackInds[i] = i;

	for(int iOrder = 0; iOrder < (int)order + 1; ++iOrder)
		for (int iDegree = -iOrder; iDegree < iOrder + 1; ++iDegree)
		{
			metadata.orders.push_back(iOrder);
			metadata.degrees.push_back(iDegree);
		}

	spaudio::Renderer admRenderer;
	bool admSuccess = admRenderer.Configure(outputLayout, order, sampleRate, nSamples, channelInfo);
	assert(admSuccess);

	AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, true, sampleRate, 0.f);
	assert(success);

	BFormat inputSignal;
	inputSignal.Configure(order, true, nSamples);
	inputSignal.Reset();
	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** hoaIn = new float* [channelInfo.nChannels];
	for (unsigned iCh = 0; iCh < channelInfo.nChannels; ++iCh)
		hoaIn[iCh] = new float[nSamples];

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		ambiEnc.SetPosition({ DegreesToRadians(az),0.f,1. });
		ambiEnc.Refresh();
		ambiEnc.Process(impulse.data(), nSamples, &inputSignal);

		for (unsigned i = 0; i < channelInfo.nChannels; ++i)
			inputSignal.ExtractStream(hoaIn[i], i, nSamples);

		admRenderer.AddHoa(hoaIn, nSamples, metadata);
		admRenderer.GetRenderedAudio(ldspkOut, nSamples);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		{
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
				std::cout << ldspkOut[iLdspk][iSamp] << ", ";
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
	for (unsigned iCh = 0; iCh < channelInfo.nChannels; ++iCh)
		delete hoaIn[iCh];
	delete[] hoaIn;
}

/** Test 2D HOA decoding
*/
void test2dHoaDecoding()
{
	unsigned nSamples = 128;
	unsigned order = 2;
	bool b3D = true;
	unsigned sampleRate = 48000;
	auto layout = Amblib_SpeakerSetUps::kAmblib_HexagonWithCentre;

	AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, b3D, sampleRate, 0.f);
	assert(success);

	AmbisonicDecoder ambiDec;
	success = ambiDec.Configure(order, b3D, nSamples, sampleRate, layout);
	assert(success);

	unsigned nLdspk = ambiDec.GetSpeakerCount();

	BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;
	std::vector<std::vector<float>> output(inputSignal.GetChannelCount(), std::vector<float>(nSamples, 0.f));

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	ambiEnc.SetPosition({ DegreesToRadians(30.f),0.f,1. });
	ambiEnc.Refresh();
	ambiEnc.Process(impulse.data(), nSamples, &inputSignal);

	ambiDec.Process(&inputSignal, nSamples, ldspkOut);

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
	{
		for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
			std::cout << ldspkOut[iLdspk][iSamp] << ", ";
		std::cout << std::endl;
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the loudspeaker decoder presets
*/
void testAllRAD()
{
	unsigned nSamples = 1;
	unsigned order = 3;
	bool b3D = true;
	unsigned sampleRate = 48000;
	auto layout = Amblib_SpeakerSetUps::kAmblib_71;

	AmbisonicEncoder ambiEnc;
	bool success = ambiEnc.Configure(order, b3D, sampleRate, 0.f);
	assert(success);

	AmbisonicAllRAD ambiDec;
	success = ambiDec.Configure(order, nSamples, sampleRate, "0+5+0");
	assert(success);

	unsigned nLdspk = ambiDec.GetSpeakerCount();

	BFormat inputSignal;
	inputSignal.Configure(order, b3D, nSamples);
	inputSignal.Reset();
	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		ambiEnc.SetPosition({ DegreesToRadians(az),0.f,1. });
		ambiEnc.Refresh();
		ambiEnc.Process(impulse.data(), nSamples, &inputSignal);

		ambiDec.Process(&inputSignal, nSamples, ldspkOut);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		{
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
				std::cout << ldspkOut[iLdspk][iSamp] << ", ";
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the AdmRenderer
*/
void testObjectPanner()
{
	unsigned nSamples = 1;
	unsigned sampleRate = 48000;
	auto layout = spaudio::OutputLayout::ThirteenPointOne;

	spaudio::ObjectPanner objPanner;
	objPanner.Configure(layout, sampleRate, (float)nSamples / (float)sampleRate);
	unsigned nLdspk = (unsigned)objPanner.GetNumSpeakers();

	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		auto position = PolarPosition<double>{ az, 0.f, 1.f };
		objPanner.SetPosition(position);
		objPanner.Process(impulse.data(), nSamples, ldspkOut, nSamples);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		{
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
				std::cout << std::setprecision(12) << ldspkOut[iLdspk][iSamp] << ", ";
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the AdmRenderer
*/
void testAdmRenderer()
{
	unsigned nSamples = 1;
	unsigned order = 1;
	unsigned sampleRate = 48000;
	auto layout = spaudio::OutputLayout::ThirteenPointOne;
	StreamInformation streamInfo;
	streamInfo.nChannels = 1;
	streamInfo.typeDefinition = { TypeDefinition::Objects };

	spaudio::Renderer renderer;
	renderer.Configure(layout, order, sampleRate, nSamples, streamInfo);
	auto nLdspk = renderer.GetSpeakerCount();

	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	ObjectMetadata objMetadata;
	objMetadata.blockLength = nSamples;
	objMetadata.trackInd = 0;
	objMetadata.jumpPosition.flag = true;

	for (float az = 0.f; az < 360.f; az += 1.f)
	{
		objMetadata.position = PolarPosition<double>{az, 0.f, 1.f};
		renderer.AddObject(impulse.data(), nSamples, objMetadata);
		renderer.GetRenderedAudio(ldspkOut, nSamples);

		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
		{
			for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
				std::cout << std::setprecision(12) << ldspkOut[iLdspk][iSamp] << ", ";
			std::cout << std::endl;
		}
	}

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the AdmRenderer binaural output
*/
void testAdmRendererBinaural()
{
	unsigned nSamples = 1024;
	unsigned order = 1;
	unsigned sampleRate = 48000;
	auto layout = spaudio::OutputLayout::Binaural;
	StreamInformation streamInfo;
	streamInfo.nChannels = 1;
	streamInfo.typeDefinition = { TypeDefinition::Objects };

	spaudio::Renderer renderer;
	renderer.Configure(layout, order, sampleRate, nSamples, streamInfo, "", true);
	auto nLdspk = 2;

	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	ObjectMetadata objMetadata;
	objMetadata.blockLength = nSamples;
	objMetadata.trackInd = 0;
	objMetadata.position = PolarPosition<double>{90., 0.f, 1.f};
	objMetadata.jumpPosition.flag = true;

	renderer.AddObject(impulse.data(), nSamples, objMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
	{
		for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
			std::cout << std::setprecision(12) << ldspkOut[iLdspk][iSamp] << ", ";
		std::cout << std::endl;
	}

	for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

/** Test the AdmRenderer binaural output of DirectSpeakers
*/
void testAdmRendererDirectSpeakerBinaural()
{
	unsigned nSamples = 256;
	unsigned order = 1;
	unsigned sampleRate = 48000;
	auto layout = spaudio::OutputLayout::Binaural;
	StreamInformation streamInfo;
	streamInfo.nChannels = 2;
	streamInfo.typeDefinition = std::vector<TypeDefinition>(streamInfo.nChannels, TypeDefinition::DirectSpeakers);

	spaudio::Renderer renderer;
	renderer.Configure(layout, order, sampleRate, nSamples, streamInfo, "", true);
	auto nLdspk = 2;

	std::vector<float> impulse(nSamples, 0.f);
	impulse[0] = 1.f;

	float** ldspkOut = new float* [nLdspk];
	for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	DirectSpeakerMetadata speakerMetadata;
	speakerMetadata.trackInd = 0;
	speakerMetadata.polarPosition = DirectSpeakerPolarPosition{ -90., 0.f, 1.f };
	speakerMetadata.speakerLabel = "LFE";

	//renderer.AddDirectSpeaker(impulse.data(), nSamples, speakerMetadata);

	impulse[0] = 0.f;
	impulse[64] = 1.f;

	speakerMetadata.trackInd = 1;
	speakerMetadata.polarPosition = DirectSpeakerPolarPosition{ 90., 0.f, 1.f };
	speakerMetadata.speakerLabel = "M+090";

	renderer.AddDirectSpeaker(impulse.data(), nSamples, speakerMetadata);

	renderer.GetRenderedAudio(ldspkOut, nSamples);

	for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
	{
		for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
			std::cout << std::setprecision(12) << ldspkOut[iLdspk][iSamp] << ", ";
		std::cout << std::endl;
	}

	for (int iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

void testAlloPSP()
{
	Layout layout = Layout::getLayoutWithoutLFE(Layout::getMatchingLayout("4+5+0"));
	assert(layout.getNumChannels() > 0);
	spaudio::adm::AllocentricPannerGainCalc alloPSP(layout);

	CartesianPosition<double> position = { 0.,1.,0. };

	std::vector<double> gains(alloPSP.getNumChannels());

	std::vector<bool> excluded(alloPSP.getNumChannels(), false);

	alloPSP.CalculateGains(position, excluded, gains);
}

void testAlloExtent()
{
	Layout layout = Layout::getLayoutWithoutLFE(Layout::getMatchingLayout("9+10+3"));
	assert(layout.getNumChannels() > 0);
	spaudio::adm::AllocentricExtent alloExtent(layout);

	CartesianPosition<double> position = { 0.,1.,0. };

	std::vector<double> gains(alloExtent.getNumChannels());

	std::vector<bool> excluded(alloExtent.getNumChannels(), false);

	excluded[13] = true;
	excluded[19] = true;
	excluded[20] = true;
	excluded[21] = true;

	alloExtent.handle(position, 0., 0., 0., excluded, gains);
}

void testInsideAngleRange()
{
	assert(insideAngleRange(45., -180., 180., 1e-6));
	assert(insideAngleRange(-30., -90., 90., 1e-6));
	assert(insideAngleRange(-110., -120., -100.));
	assert(insideAngleRange(0., -90., 90., 0.));
	assert(!insideAngleRange(91., -90., 90., 0.));
	assert(insideAngleRange(95., -90., 90., 5.));
	assert(insideAngleRange(95., 90., -90., 0.));
	assert(!insideAngleRange(85., 90., -90., 0.));
	assert(insideAngleRange(85., 90., -90., 5.));
	assert(insideAngleRange(0., 0., 0., 0.));
	assert(!insideAngleRange(1., 0., 0., 0.));
	assert(insideAngleRange(180., 180., 180., 0.));
	assert(insideAngleRange(180., -180., -180., 0.));
	assert(!insideAngleRange(179., -180., -180., 0.));
	assert(insideAngleRange(175., 180., 180., 5.));
	assert(!insideAngleRange(170., 180., 180., 5.));
	assert(insideAngleRange(180., -180., 180., 0.));
	assert(insideAngleRange(270., -180., 180., 0.));
	assert(insideAngleRange(-90., -180., 180., 0.));
	assert(insideAngleRange(0., -180., 180., 0.));
}

void testLayoutRangeCheck()
{
	for (auto& layout : Layout::getSpeakerLayouts())
	{
		bool isValid = checkLayoutAngles(layout);
		assert(isValid);
	}

	Layout layout = Layout::getMatchingLayout("9+10+3");
	auto polPos = layout.getChannel(0).getPolarPosition();
	polPos.azimuth += 90.;
	layout.getChannel(0).setPolarPosition(polPos);
	bool isValid = checkLayoutAngles(layout);
	assert(!isValid);
}

void testAdmRenderCustomPositions()
{
	unsigned int hoaOrder = 1;
	unsigned int sampleRate = 48000;
	unsigned int nSamples = 128;
	StreamInformation streamInfo;
	Optional<Screen> screen;

	spaudio::Renderer admRenderer;
	std::vector<PolarPosition<double>> customPositions;
	spaudio::OutputLayout outputTarget = spaudio::OutputLayout::FivePointOne;
	auto layout = Layout::getMatchingLayout("0+5+0");
	customPositions.resize(layout.getNumChannels());
	for (int i = 0; i < (int)layout.getNumChannels(); ++i)
		customPositions[i] = layout.getChannel(i).getPolarPosition();

	auto success = admRenderer.Configure(outputTarget, hoaOrder, sampleRate, nSamples, streamInfo, "", true, screen, customPositions);
	assert(success);

	customPositions[0].azimuth *= -1.;
	auto failure = !admRenderer.Configure(outputTarget, hoaOrder, sampleRate, nSamples, streamInfo, "", true, screen, customPositions);
	assert(failure);
}

void testRendererStreamAndOutputGains()
{
	unsigned int hoaOrder = 1;
	unsigned int nHoa = (hoaOrder + 1) * (hoaOrder + 1);
	unsigned int sampleRate = 48000;
	unsigned int nSamples = 128;
	double gain1 = 0.25f;
	double gain2 = 0.5f;
	double endGain = gain1 + (gain2 - gain1) / (double)nSamples * (nSamples - 1); // Gain the end of a smoothing frame
	double outGain1 = 0.3f;
	double outGain2 = 0.75;
	double endOutGain = outGain1 + (outGain2 - outGain1) / (double)nSamples * (nSamples - 1); // Gain the end of a smoothing frame

	std::vector<PolarPosition<double>> customPositions;
	spaudio::OutputLayout outputTarget = spaudio::OutputLayout::FivePointOne;
	auto layout = Layout::getMatchingLayout("0+5+0");
	StreamInformation streamInfo;
	streamInfo.typeDefinition = { TypeDefinition::DirectSpeakers };
	for (auto i = 0; i < nHoa; ++i)
		streamInfo.typeDefinition.push_back(TypeDefinition::HOA);
	streamInfo.nChannels = streamInfo.typeDefinition.size();

	spaudio::Renderer renderer;
	bool success = renderer.Configure(outputTarget, hoaOrder, sampleRate, nSamples, streamInfo, "", true);
	assert(success);

	auto nLdspk = renderer.GetSpeakerCount();

	std::vector<float> ones(nSamples, 1.f);

	float** ldspkOut = new float* [nLdspk];
	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		ldspkOut[iLdspk] = new float[nSamples];

	DirectSpeakerMetadata dirSpkMetadata;
	dirSpkMetadata.trackInd = 0;
	dirSpkMetadata.gain = gain1;
	dirSpkMetadata.speakerLabel = "M+000";

	// Process the DirectSpeaker first
	renderer.AddDirectSpeaker(ones.data(), nSamples, dirSpkMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	// First call so first and last samples should be the target gain
	assert(ldspkOut[2][0] == static_cast<float>(gain1) && ldspkOut[2][nSamples - 1] == static_cast<float>(gain1));
	dirSpkMetadata.gain = gain2;
	renderer.AddDirectSpeaker(ones.data(), nSamples, dirSpkMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	assert(ldspkOut[2][0] == static_cast<float>(gain1) && ldspkOut[2][nSamples - 1] == static_cast<float>(endGain));

	HoaMetadata hoaMetadata;
	hoaMetadata.normalization = "SN3D";
	hoaMetadata.trackInds.resize(nHoa);
	hoaMetadata.gain = gain1;

	for (unsigned i = 0; i < nHoa; ++i)
	{
		hoaMetadata.trackInds[i] = i + 1; // Add 1 to account for DirectSpeaker stream
	}

	for (int iOrder = 0; iOrder < (int)hoaOrder + 1; ++iOrder)
		for (int iDegree = -iOrder; iDegree < iOrder + 1; ++iDegree)
		{
			hoaMetadata.orders.push_back(iOrder);
			hoaMetadata.degrees.push_back(iDegree);
		}

	float** hoaIn = new float* [nHoa];
	for (unsigned iCh = 0; iCh < nHoa; ++iCh)
	{
		hoaIn[iCh] = new float[nSamples];
		for (unsigned iSamp = 0; iSamp < nSamples; ++iSamp)
			hoaIn[iCh][iSamp] = iCh == 0 ? 1.f : 0.f; // Set up as an omni (order zero) signal since we're only testing gain smoothing
	}

	// Process the HOA Channels
	renderer.AddHoa(hoaIn, nSamples, hoaMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	// No smoothing so first and last samples should match
	assert(ldspkOut[2][0] == ldspkOut[2][nSamples - 1]);
	hoaMetadata.gain = gain2;
	renderer.AddHoa(hoaIn, nSamples, hoaMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	assert(std::abs(ldspkOut[2][nSamples - 1] / ldspkOut[2][0] - static_cast<float>(endGain / gain1)) < 1e-6);

	// Test the output gain
	renderer.Reset();
	renderer.SetOutputGain(outGain1);
	dirSpkMetadata.gain = 1.;
	renderer.AddDirectSpeaker(ones.data(), nSamples, dirSpkMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	// First call so first and last samples should be the target gain
	assert(ldspkOut[2][0] == static_cast<float>(outGain1) && ldspkOut[2][nSamples - 1] == static_cast<float>(outGain1));
	renderer.SetOutputGain(outGain2);
	renderer.AddDirectSpeaker(ones.data(), nSamples, dirSpkMetadata);
	renderer.GetRenderedAudio(ldspkOut, nSamples);
	assert(ldspkOut[2][0] == static_cast<float>(outGain1) && ldspkOut[2][nSamples - 1] == static_cast<float>(endOutGain));

	for (unsigned iLdspk = 0; iLdspk < nLdspk; ++iLdspk)
		delete ldspkOut[iLdspk];
	delete[] ldspkOut;
}

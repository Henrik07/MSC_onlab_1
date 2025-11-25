#include "WavetableOsc.h"
#include <cmath>

WavetableOsc::WavetableOsc(std::vector<float> waveTable, double sampleRate)
	: waveTable{ std::move(waveTable) }, sampleRate{ sampleRate } {}

void WavetableOsc::setFrequency(float frequency) {
	indexIncrement = frequency * static_cast<float>(waveTable.size()) / static_cast<float>(sampleRate);
}

float WavetableOsc::getSample() {
	const auto sample = interpolateLinear();

	index += indexIncrement;
	index = std::fmod(index, static_cast<float>(waveTable.size()));
	
	return sample;
}

float WavetableOsc::interpolateLinear() {
	const auto truncatedIndex = static_cast<int>(index);
	const auto nextIndex = (truncatedIndex + 1) % static_cast<int>(waveTable.size());

	const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
	const auto truncatedIndexWeight = 1.f - nextIndexWeight;

	return truncatedIndexWeight * waveTable[truncatedIndex] + nextIndexWeight * waveTable[nextIndex];
}

void WavetableOsc::stop() {
	index = 0.f;
	indexIncrement = 0.f;
}

bool WavetableOsc::isPlaying() {
	return indexIncrement != 0.f;
}
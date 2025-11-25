#include "DPW_Osc.h"
#include <cmath>

DPW_Osc::DPW_Osc(double sampleRate) : sampleRate{ sampleRate } {}

void DPW_Osc::setFrequency(float frequency) {
	phaseIncrement = frequency / static_cast<float>(sampleRate);
}

float DPW_Osc::getSample() {
	const auto sample = phase * 2.f - 1.f;

	phase += phaseIncrement;
	phase = std::fmod(phase, 1.f);

	return sample;
}

void DPW_Osc::stop() {
	phase = 0.f;
	phaseIncrement = 0.f;
}

bool DPW_Osc::isPlaying() {
	return phaseIncrement != 0.f;
}
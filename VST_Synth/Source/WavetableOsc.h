#pragma once
#include <vector>

class WavetableOsc {
public:
	WavetableOsc(std::vector<float> waveTabble, double sampleRate);

	void setFrequency(float frequency);
	void stop();

	bool isPlaying();

	float getSample();

private:
	std::vector<float> waveTable;

	double sampleRate;

	float frequency;
	float index = 0.f;
	float indexIncrement = 0.f;
	float interpolateLinear();
};


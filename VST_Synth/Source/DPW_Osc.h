#pragma once
#include <vector>

class DPW_Osc {
public:
	DPW_Osc(double sampleRate);

	void setFrequency(float frequency);
	void stop();

	bool isPlaying();

	float getSample();

private:
	double sampleRate;

	float frequency;
	float phase = 0.f;
	float phaseIncrement = 0.f;
};
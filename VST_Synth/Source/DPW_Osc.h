#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>

constexpr auto pi = M_PI;

class DPW_Osc {
public:
	DPW_Osc(double sampleRate);

	void setDPW(bool dpw);
	void setFrequency(float frequency);
	void stop();

	bool isPlaying();

	float getSample();

private:
	bool dpw;

	double sampleRate;

	float frequency;
	float phase = 0.f;
	float phaseIncrement = 0.f;
};
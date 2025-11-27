#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "JuceHeader.h"
#include <math.h>
#include <stdio.h>
#include "DPW_Osc.h"

constexpr auto b0 = 1.0f;
constexpr auto b1 = -1.f;

constexpr auto a1 = 1.0f;

//constexpr auto pi = M_PI;

constexpr auto OSCILLATORS_COUNT = 128;

class DPW_Synth{
public:
	void setDPW(bool dpw);

	void setVolume(float volume);

	void prepareToPlay(double sampleRate);

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

	void renderSawtooth(juce::AudioBuffer<float>& buffer);

	void renderSawtooth2(juce::AudioBuffer<float>& buffer, int startSample, int endSample);

private:
	std::vector<DPW_Osc> oscillators;

	bool dpw;

	std::vector <std::array<float, 2>> uBuffer;
	std::vector <std::array<float, 2>> yBuffer;

	double sampleRate;

	float frequency = 0.f;

	float volume;

	void handleMidiEvent(const juce::MidiMessage& midiEvent);

	float midiNoteNumberToFrequency(int midiNoteNumber);

	void initializeOscillators();
};


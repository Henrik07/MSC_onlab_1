#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "JuceHeader.h"
#include <math.h>
#include <stdio.h>
#include "DPW_Osc.h"

//constexpr float b0 = 1.0f;
//constexpr float b1 = -1.f;

//constexpr float a1 = 1.0f;

constexpr auto pi = M_PI;

class DPW_Synth{
public:
	void setVolume(float volume);

	void prepareToPlay(double sampleRate);

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

	void renderSawtooth(juce::AudioBuffer<float>& buffer);

	void renderSawtooth2(juce::AudioBuffer<float>& buffer, int startSample, int endSample);

private:
	std::vector<DPW_Osc> oscillators;

	double sampleRate;

	float frequency = 0.f;

	float volume;

	int counter;

	int period;

	void handleMidiEvent(const juce::MidiMessage& midiEvent);

	float midiNoteNumberToFrequency(int midiNoteNumber);

	void initializeOscillators();
};


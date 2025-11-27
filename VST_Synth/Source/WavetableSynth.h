#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "JuceHeader.h"
#include <math.h>
#include <stdio.h>
#include "WavetableOsc.h"

//constexpr float b0 = 1.0f;
//constexpr float b1 = -1.f;

//constexpr float a1 = 1.0f;

class WavetableSynth {
public:
	void setVolume(float volume);

	void prepareToPlay(double sampleRate);

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

private:
	std::vector<WavetableOsc> oscillators;
	std::vector<float> generateWaveTable();

	double sampleRate;

	void handleMidiEvent(const juce::MidiMessage& midiEvent);
	void initializeOscillators();
	void render(juce::AudioBuffer<float>& buffer, int startSample, int endSample);

	float midiNoteNumberToFrequency(int midiNoteNumber);
	float volume;
};


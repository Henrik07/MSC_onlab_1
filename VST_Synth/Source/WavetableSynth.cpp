#include "WavetableSynth.h"

//const auto pi = M_PI;

void WavetableSynth::setVolume(float volume) {
	this->volume = volume;
}

/*
float renderSawtooth(float* buffer, int numSamples, float frequency, float sampleRate) {
    double phase = 0;
    double phaseIncrement = 2 * pi * frequency / sampleRate;

    for (int i = 0; i < numSamples; ++i) {
        double sample = 0;
        for (int n = 1; n <= 10; ++n) { // Add 10 harmonics (adjust as needed)
            double harmonicFrequency = frequency * n;
            double harmonicPhase = phase * n;
            double amplitude = 1.0 / n;
            
            // Alternate sign for even harmonics
            if (n % 2 == 0) {
                sample += amplitude * std::sin(harmonicPhase);
            } else {
                sample += amplitude * std::sin(harmonicPhase);
            }
        }
        buffer[i] = static_cast<float>(sample);
        phase += phaseIncrement;
    }
}*/

/*std::vector<float> WavetableSynth::generateSawWaveTable(std::vector<float>& sineWaveTable, double sampleRate) {
	constexpr auto WAVETABLE_LENGTH = 128;

	std::vector<float> sawWaveTable(WAVETABLE_LENGTH);

	double phase = 0;
	double phaseIncrement = 2 * pi / sampleRate;

	for (auto i = 0; i < WAVETABLE_LENGTH; ++i) {
		double sample = 0;
		for (int n = 1; n <= 10; ++n) { // Add 10 harmonics (adjust as needed)
			double harmonicFrequency = static_cast<float>(i) * n;
			double harmonicPhase = phase * n;
			double amplitude = 1.0 / n;

			// Alternate sign for even harmonics
			if (n % 2 == 0) {
				sample += amplitude * std::sin(harmonicPhase);
			}
			else {
				sample += amplitude * std::sin(harmonicPhase);
			}
		}
		sawWaveTable[i] = static_cast<float>(sample);
		phase += phaseIncrement;
	}

	return sawWaveTable;
}*/

std::vector<float> WavetableSynth::generateWaveTable() {
	constexpr auto WAVETABLE_LENGTH = 128;

	std::vector<float> sineWaveTable(WAVETABLE_LENGTH);

	//std::vector<float> sawWaveTable(WAVETABLE_LENGTH);

	const auto pi = M_PI;

	for (auto i = 0; i < WAVETABLE_LENGTH; ++i) {
		sineWaveTable[i] = std::sinf(2 * pi * static_cast<float>(i) / static_cast<float>(WAVETABLE_LENGTH));
	}

	//sawWaveTable = generateSawWaveTable(sineWaveTable, sampleRate);

	return sineWaveTable;
	//return sawWaveTable;
}

void WavetableSynth::initializeOscillators() {
	constexpr auto OSCILLATORS_COUNT = 128;

	const auto waveTable = generateWaveTable();

	oscillators.clear();

	for (auto i = 0; i < OSCILLATORS_COUNT; ++i) {
		oscillators.emplace_back(waveTable, sampleRate);
	}
}

void WavetableSynth::prepareToPlay(double sampleRate) {
	this->sampleRate = sampleRate;

	initializeOscillators();
}

void WavetableSynth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	
	auto currentSample = 0;

	for (const auto midiMessage : midiMessages) {
		
		const auto midiEvent = midiMessage.getMessage();
		const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

		render(buffer, currentSample, midiEventSample);
		
		handleMidiEvent(midiEvent);

		currentSample = midiEventSample;
	}

	render(buffer, currentSample, buffer.getNumSamples());

	/*const auto pi = M_PI;
	dnBuffer.resize(buffer.getNumChannels(), 0.f);
	const auto sign = highPass ? -1.f : 1.f; //HPF (-1) or LPF (1)

	const auto tan = std::tan(pi * cutoffFrequency / samplingRate);
	const auto a1 = (tan - 1.f) / (tan + 1.f); //filter coefficient

	for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
		auto channelSamples = buffer.getWritePointer(channel);
		for (auto i = 0; i < buffer.getNumSamples(); ++i) {
			const auto inputSample = channelSamples[i];

			const auto allPassFilteredSample = a1 * inputSample + dnBuffer[channel];
			dnBuffer[channel] = inputSample - a1 * allPassFilteredSample;

			const auto filterOutput = 0.5f * (inputSample + sign * allPassFilteredSample);

			channelSamples[i] = filterOutput;
		}
	}*/
}

void WavetableSynth::render(juce::AudioBuffer<float>& buffer, int startSample, int endSample) {
	auto* firstChannel = buffer.getWritePointer(0);

	for (auto& oscillator : oscillators) {
		if (oscillator.isPlaying()) {
			for (auto sample = startSample; sample < endSample; ++sample) {
				firstChannel[sample] += oscillator.getSample();
				firstChannel[sample] *= pow(volume, 2);
			}
		}
	}

	for (auto channel = 1; channel < buffer.getNumChannels(); ++channel) {
		auto* channelData = buffer.getWritePointer(channel);

		std::copy(firstChannel + startSample, firstChannel + endSample, 
			channelData + startSample);
		
		/*for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
			channelData[sample] *= volume;
		}*/
	}
}

void WavetableSynth::handleMidiEvent(const juce::MidiMessage& midiEvent) {
	
	if (midiEvent.isNoteOn()) {
		const auto oscillatorId = midiEvent.getNoteNumber();
		const auto frequency = midiNoteNumberToFrequency(oscillatorId);
		oscillators[oscillatorId].setFrequency(frequency);
	}

	else if (midiEvent.isNoteOff()) {
		const auto oscillatorId = midiEvent.getNoteNumber();
		oscillators[oscillatorId].stop();
	}
	else if (midiEvent.isAllNotesOff()) {
		for (auto& oscillator : oscillators) {
			oscillator.stop();
		}
	}
}

float WavetableSynth::midiNoteNumberToFrequency(int midiNoteNumber) {

	constexpr auto A4_FREQUENCY = 440.f;
	constexpr auto A4_NOTE_NUMBER = 69.f;
	constexpr auto SEMITONES_IN_AN_OCTAVE = 12.f;
	return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}
#include "DPW_Synth.h"

void DPW_Synth::setVolume(float volume) {
    this->volume = volume;
}


void DPW_Synth::initializeOscillators(){
    constexpr auto OSCILLATORS_COUNT = 128;

    oscillators.clear();

    for (auto i = 0; i < OSCILLATORS_COUNT; ++i) {
        oscillators.emplace_back(sampleRate);
    }
}

void DPW_Synth::prepareToPlay(double sampleRate) {
    this->sampleRate = sampleRate;
    initializeOscillators();
}

void DPW_Synth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {

    auto currentSample = 0;

    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

        //renderSawtooth(buffer);
        renderSawtooth2(buffer, currentSample, midiEventSample);

        handleMidiEvent(midiEvent);

        currentSample = midiEventSample;
    }

    //renderSawtooth(buffer);
    renderSawtooth2(buffer, currentSample, buffer.getNumSamples());
}

void DPW_Synth::renderSawtooth(juce::AudioBuffer<float>& buffer) {
    float phase = 0;
    float phaseIncrement = frequency / sampleRate;
    /*
    double period = 2 * pi * frequency / sampleRate;

    for (int i = 0; i < buffer.getNumSamples(); i++) {
        float output = (float)counter / period * 2 - 1;
        counter = (counter + 1) % period;
        *buffer.getWritePointer(0, i) = output;
    }

    for (int channel = 1; channel < buffer.getNumChannels(); ++channel)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            *buffer.getWritePointer(channel, i) = *buffer.getWritePointer(0, i);
        }
    }*/
    
    auto numSamples = buffer.getNumSamples();

    //for (auto channel = 1; channel < buffer.getNumChannels(); ++channel) {
       // auto* channelData = buffer.getWritePointer(channel);
        
        /*auto sample_val = 0;
        for (int n = 1; n <= 10; ++n) { // Add 10 harmonics (adjust as needed)
            double harmonicPhase = phase * n;
            double amplitude = 1.0 / n;

            sample_val += amplitude * std::sin(harmonicPhase);
        }*/

        for (int sample = 0; sample < numSamples; sample++) {
            //phase += phaseIncrement;
            //auto output = sin(2 * pi * phase);
            //if (phase >= 1) { phase -= 2; }
            phase = sample * phaseIncrement;
            auto output = fmod(phase,1) * 2 - 1;
            output *= pow(volume, 2);
            *buffer.getWritePointer(0, sample) = output;
        }

        for (int channel = 1; channel < buffer.getNumChannels(); ++channel)
        {
            for (int sample = 0; sample < numSamples; sample++) {
                *buffer.getWritePointer(channel, sample) = *buffer.getWritePointer(0, sample);
            }
        }
    //}
}

void DPW_Synth::renderSawtooth2(juce::AudioBuffer<float>& buffer, int startSample, int endSample) {

    auto* firstChannel = buffer.getWritePointer(0);

    for (auto& oscillator : oscillators) {
        if (oscillator.isPlaying()) {
            for (auto sampleNum = startSample; sampleNum < endSample; ++sampleNum) {

                firstChannel[sampleNum] += oscillator.getSample();
                firstChannel[sampleNum] *= pow(volume, 2);

            }
        }
    }

    for (auto channel = 1; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        std::copy(firstChannel + startSample, firstChannel + endSample, channelData + startSample);
    }
}

void DPW_Synth::handleMidiEvent(const juce::MidiMessage& midiEvent) {

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

float DPW_Synth::midiNoteNumberToFrequency(int midiNoteNumber) {

    constexpr auto A4_FREQUENCY = 440.f;
    constexpr auto A4_NOTE_NUMBER = 69.f;
    constexpr auto SEMITONES_IN_AN_OCTAVE = 12.f;
    return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}
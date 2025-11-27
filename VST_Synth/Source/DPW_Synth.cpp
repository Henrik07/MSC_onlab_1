#include "DPW_Synth.h"

void DPW_Synth::setVolume(float volume) {
    this->volume = volume;
}

void DPW_Synth::setDPW(bool dpw) {
    this->dpw = dpw;
}

void DPW_Synth::initializeOscillators(){
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

    auto scaling_factor = sampleRate / (4 * frequency);

    auto* firstChannel = buffer.getWritePointer(0);

    for (auto& oscillator : oscillators) {
        if (oscillator.isPlaying()) {
            for (auto sampleNum = startSample; sampleNum < endSample; ++sampleNum) {

                firstChannel[sampleNum] += oscillator.getSample();

                if (dpw) {
                    firstChannel[sampleNum] *= firstChannel[sampleNum]; //raising each sample to the power of 2

                    ////////////////////////// MATLAB filter
                    float tmp = firstChannel[sampleNum];
                    auto filterOut = b0 * firstChannel[sampleNum] + b1 * uBuffer[0][0] - a1 * yBuffer[0][0];
                    firstChannel[sampleNum] = b0 * firstChannel[sampleNum] + b1 * uBuffer[0][0] - a1 * yBuffer[0][0];
                    //if (filterOut > 1) { firstChannel[sampleNum] = 1; }
                    //else { firstChannel[sampleNum] = filterOut; }

                    yBuffer[0][0] = firstChannel[sampleNum];
                    uBuffer[0][0] = tmp;
                    //////////////////////////

                    if (firstChannel[sampleNum] * scaling_factor > 1) {
                        firstChannel[sampleNum] = 1;
                    }
                    else {
                        firstChannel[sampleNum] *= scaling_factor;
                    }
                }

                firstChannel[sampleNum] *= pow(volume, 2);
            }
            /*for (auto sampleNum = startSample; sampleNum < endSample; ++sampleNum) {
                if (dpw) {
                    firstChannel[sampleNum] *= firstChannel[sampleNum]; //raising each sample to the power of 2

                    ////////////////////////// MATLAB filter
                    float tmp = firstChannel[sampleNum];
                    auto filterOut = b0 * firstChannel[sampleNum] + b1 * uBuffer[0][0] - a1 * yBuffer[0][0];
                    //firstChannel[sampleNum] = b0 * firstChannel[sampleNum] + b1 * uBuffer[0][0] - a1 * yBuffer[0][0];
                    if (filterOut > 1) { firstChannel[sampleNum] = 1; }
                    else { firstChannel[sampleNum] = filterOut; }

                    yBuffer[0][0] = firstChannel[sampleNum];
                    uBuffer[0][0] = tmp;
                    //////////////////////////

                    if (firstChannel[sampleNum] * scaling_factor > 1) {
                        firstChannel[sampleNum] = 1;
                    }
                    else {
                        firstChannel[sampleNum] *= scaling_factor;
                    }
                }
                firstChannel[sampleNum] *= pow(volume, 2);
            }*/
        }
    }

    //auto scaling_factor = sampleRate / (4 * frequency);

    for (auto channel = 1; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        std::copy(firstChannel + startSample, firstChannel + endSample, channelData + startSample);

        /*if (dpw) {
            for (auto sampleNum = startSample; sampleNum < endSample; ++sampleNum) {
                channelData[sampleNum] *= channelData[sampleNum]; //raising each sample to the power of 2

                ////////////////////////// MATLAB filter
                float tmp = channelData[sampleNum];
                channelData[sampleNum] = a1 * yBuffer[channel][0] + b0 * channelData[sampleNum] + b1 * uBuffer[channel][0];

                yBuffer[channel][0] = channelData[sampleNum];
                uBuffer[channel][0] = tmp;
                //////////////////////////

                channelData[sampleNum] *= scaling_factor;
            }

            //for (auto sampleNum = startSample; sampleNum < endSample; ++sampleNum) {
            //    channelData[sampleNum] *= scaling_factor;
            //}
        }*/
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
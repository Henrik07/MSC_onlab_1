/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p, 
    juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(volumeSlider);
    volumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        vts, "volume", volumeSlider));

    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    volumeSlider.setBounds(20, 35, 300, 50);

    volumeLabel.setBounds(volumeSlider.getWidth() / 2,
        volumeSlider.getY() - 30,
        300, 50);
}

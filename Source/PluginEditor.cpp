/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQThroughWallsAudioProcessorEditor::EQThroughWallsAudioProcessorEditor (EQThroughWallsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

EQThroughWallsAudioProcessorEditor::~EQThroughWallsAudioProcessorEditor()
{
}

//==============================================================================
void EQThroughWallsAudioProcessorEditor::paint (juce::Graphics& g)
{


    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.setFont(juce::Font("Times New Roman", 20.0f, juce::Font::italic));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    g.setColour(juce::Colours::green);
    g.drawLine(10, 300, 590, 300, 5);
}

void EQThroughWallsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

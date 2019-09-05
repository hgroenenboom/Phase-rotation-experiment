/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FftphaseRotationAudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    FftphaseRotationAudioProcessorEditor (FftphaseRotationAudioProcessor&);
    ~FftphaseRotationAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override {
		repaint();
	}
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	FftphaseRotationAudioProcessor & processor;
	FftphaseRotationAudioProcessor& p;

	DrawableText combobox_t;
	DrawableText combobox2_t;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftphaseRotationAudioProcessorEditor)
};

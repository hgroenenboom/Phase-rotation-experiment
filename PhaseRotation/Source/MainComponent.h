/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

	void timerCallback() override {
		repaint();
	}

	dsp::FFT fft;
private:
    //==============================================================================
    // Your private member variables go here...
	static const int fftOrder = 10;
	static const int fftSize = 1 << fftOrder;
	static const int normalizationFactor = 1 << ((fftOrder-1)*2);
	static const int fftBufferSize = fftSize << 1;

	float fftBuffer[fftBufferSize];
	float audioBuffer[fftSize];

	float saw[fftSize];
	float sawMags[fftSize];
	float sawPhas[fftSize];

	float drawPhas[fftSize];
	float drawMags[fftSize];

	ToggleButton onlyHarmonics;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

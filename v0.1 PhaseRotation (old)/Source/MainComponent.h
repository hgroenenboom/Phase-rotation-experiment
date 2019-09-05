/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>

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

	dsp::FFT* fft = nullptr;
private:
    //==============================================================================
    // Your private member variables go here...
	int fftOrder = 5;   // 5 - 7 - 10
	int fftSize = 1 << fftOrder;
	int normalizationFactor = 1 << ((fftOrder-1)*2);
	int fftBufferSize = fftSize << 1;

	static const int maxFFTOrder = 11;
	static const int maxFFTSize = 1 << maxFFTOrder;

	float fftBuffer[maxFFTSize];
	std::vector<float> audioBuffer;

	std::vector<float> saw;
	std::vector<float> sawMags;
	std::vector<float> sawPhas;

	std::vector<float> drawPhas;
	std::vector<float> drawMags;

	ToggleButton onlyHarmonics;
	ComboBox fftSizeSetting;

	void fftSizeSettingCallback() {
		fftOrder = fftSizeSetting.getSelectedId();
		calculateFFTSizeAndResizeBuffers(fftOrder);
	}

	void calculateFFTSizeAndResizeBuffers(int order) {
		fftOrder = order;

		fftSize = 1 << fftOrder;
		normalizationFactor = 1 << ( 2 * (fftOrder-1) );
		fftBufferSize = fftSize << 1;
		
		if (fftSize > audioBuffer.size()) {
			audioBuffer.resize(fftSize);
			saw.resize(fftSize);
			sawMags.resize(fftSize);
			sawPhas.resize(fftSize);
			drawPhas.resize(fftSize);
			drawMags.resize(fftSize);
		}

		if (fft != nullptr)
			delete fft;
		fft = new dsp::FFT(fftOrder);
	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

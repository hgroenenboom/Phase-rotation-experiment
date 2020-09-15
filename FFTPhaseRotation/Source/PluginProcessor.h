/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include "SmoothIIRFloat.h"

//==============================================================================
/**
*/
class FftphaseRotationAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FftphaseRotationAudioProcessor();
    ~FftphaseRotationAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	// DSP and memory
    dsp::FFT* fft = nullptr;

	int fftOrder = 5;   // 5 - 7 - 10
	int fftSize = 1 << fftOrder;
	int normalizationFactor = 1 << ((fftOrder-1)*2);
	int fftBufferSize = fftSize << 1;

	static const int maxFFTOrder = 13;
	static const int maxFFTSize = 1 << maxFFTOrder;

	float fftBuffer[maxFFTSize];
	std::vector<float> audioBuffer;

	std::vector<float> saw;
	std::vector<float> sawMags;
	std::vector<float> sawPhas;

	std::vector<float> drawPhas;
	std::vector<float> drawMags;

	// UI
	LookAndFeel_V4 lookandfeel;
	ToggleButton onlyHarmonics;
	ComboBox fftSizeSetting;
	ComboBox waveformSetting;

	// Realtime variables
	float frequency = 100.0f;
	IIRFloat smoothFrequency;
	bool isGeneratingWaveform = false;
	
	// Playback settings (frequency)
	const int playbackOscillationTime = 1024;
	const float invplaybackOscillationTime = 1.0f / (float)playbackOscillationTime;

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
		fft = nullptr;
		fft = new dsp::FFT(fftOrder);
	}

	void generateWaveform() {
		isGeneratingWaveform = true;
		const int waveType = waveformSetting.getSelectedId();
		const float sawPlaybackFreq = smoothFrequency.getUnfiltered() * getSampleRate() / playbackOscillationTime;
		Random r;

		if (waveType == 2) {
			for (int i = 0; i < fftSize; i++) {
				// SIN
				saw[i] = std::sin(MathConstants<float>::twoPi * 2.0f * (0.5f * i / (float)fftSize) * smoothFrequency.getUnfiltered()); // sin
			}
		}
		else if (waveType == 1) {
			for (int i = 0; i < fftSize; i++) {
				// SAW
				saw[i] = 0.0f;
				for (int j = 1; j < 0.5 * fftSize; j++) {
					// if harmonic is under nyquist
					const float harmonicFreq = smoothFrequency.getUnfiltered() * j;
					if (harmonicFreq <= 0.5 * fftSize) {
						// add harmonic to sample
						saw[i] += (1.0f / ((float)j * 2.0f)) * sin(MathConstants<float>::twoPi * (smoothFrequency.getUnfiltered() * (float)j * (float)i / (float)fftSize));
					}
				}
			}
		}
		else if (waveType == 3) {
			// NOISE
			r.setSeed(235236);
			for (int i = 0; i < fftSize; i++) {
				saw[i] = 0.6f * (r.nextBool() ? 1.0f : -1.0f) * r.nextFloat();
			}
		}
		else if (waveType == 4) {
			for (int i = 0; i < fftSize; i++) {
				// SAW
				saw[i] = 0.0f;
				for (int j = 1; j < 0.5 * fftSize; j++) {
					// if harmonic is under nyquist
					const float harmonicFreq = smoothFrequency.getUnfiltered() * j;
					if (harmonicFreq <= 0.5 * fftSize) {
						// add harmonic to sample
						saw[i] += 0.5f * ( 1.0f / pow((float)j, 2.0f) ) * sin(MathConstants<float>::twoPi * (smoothFrequency.getUnfiltered() * (float)j * (float)i / (float)fftSize));
					}
				}
			}
		}
		else if (waveType == 5) {
			for (int i = 0; i < fftSize; i++) {
				// SQUARE
				saw[i] = 0.0f;
				for (int j = 1; j < 0.5 * fftSize; j+=2) {
					// if harmonic is under nyquist
					const float harmonicFreq = smoothFrequency.getUnfiltered() * j;
					if (harmonicFreq <= 0.5 * fftSize) {
						// add harmonic to sample
						saw[i] += (1.0f / (2.0f * j)) * sin(MathConstants<float>::twoPi * (smoothFrequency.getUnfiltered() * (float)j * (float)i / (float)fftSize));
					}
				}
			}
		}
		else if (waveType == 6) {
			for (int i = 0; i < fftSize; i++) {
				// SQUARE
				saw[i] = 0.0f;
				for (int j = 1; j < 0.5 * fftSize; j += 2) {
					// if harmonic is under nyquist
					const float harmonicFreq = smoothFrequency.getUnfiltered() * j;
					if (harmonicFreq <= 0.5 * fftSize) {
						// add harmonic to sample
						saw[i] += 0.5f * (1.0f / pow((float)j, 2.0f)) * sin(MathConstants<float>::twoPi * (smoothFrequency.getUnfiltered() * (float)j * (float)i / (float)fftSize));
					}
				}
			}
		}
		else if (waveType == 7) {
			for (int i = 0; i < fftSize; i++) {
				// SQUARE
				saw[i] = 0.0f;
				for (int j = 1; j < 0.5 * fftSize; j += 2) {
					// if harmonic is under nyquist
					const float harmonicFreq = smoothFrequency.getUnfiltered() * j;
					if (harmonicFreq <= 0.5 * fftSize) {
						// add harmonic to sample
						saw[i] += 0.5f * (1.0f / pow((float)j, 1.5f)) * sin(MathConstants<float>::twoPi * (smoothFrequency.getUnfiltered() * (float)j * (float)i / (float)fftSize));
					}
				}
			}
		}

		for (int i = 0; i < fftSize; i++) {
			// AMP
			saw[i] *= 0.5f;
		}
		
		isGeneratingWaveform = false;
	}

	float readAudioBufferInterpolated(float pha) const {
		const float pos = pha * (fftSize-1);
		const int minPos = floor(pos);
		const int maxPos = ceil(pos);
		const int am = pos - (float)minPos;
		return am*audioBuffer[ maxPos ] + (1.0f-am)*audioBuffer[ minPos ];
	}

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftphaseRotationAudioProcessor)
};

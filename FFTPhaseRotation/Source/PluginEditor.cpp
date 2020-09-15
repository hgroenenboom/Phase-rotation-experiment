/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

//==============================================================================
FftphaseRotationAudioProcessorEditor::FftphaseRotationAudioProcessorEditor (FftphaseRotationAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), p(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    startTimerHz(60);
	addAndMakeVisible(&p.onlyHarmonics);
	p.onlyHarmonics.setToggleState(true, false);
	addAndMakeVisible(&p.fftSizeSetting);
	addAndMakeVisible(&p.waveformSetting);

	combobox_t.setText("select fft size");
	combobox_t.setJustification(Justification::centred);
	combobox_t.setColour(Colours::aqua.darker(0.4f));
	combobox_t.setAlwaysOnTop(true);
	combobox_t.setFont(combobox_t.getFont(), 12.0f);
	addAndMakeVisible(&combobox_t);

	combobox2_t.setText("select waveform");
	combobox2_t.setJustification(Justification::centred);
	combobox_t.setColour(Colours::aqua.darker(0.4f));
	combobox2_t.setAlwaysOnTop(true);
	combobox2_t.setFont(combobox_t.getFont(), 12.0f);
	addAndMakeVisible(&combobox2_t);

	const int w = 1430;
	const int h = 565;
	const float res = (float)w / (float)h;

	setResizable(true, true);
	setResizeLimits(res * 300, 300, res * 1080, 1080);
	addAndMakeVisible(*resizableCorner);
	setSize(1430, 565);
}

FftphaseRotationAudioProcessorEditor::~FftphaseRotationAudioProcessorEditor()
{
}

float mouseX = 0.0f;
float mouseY = 0.0f;
//==============================================================================
void FftphaseRotationAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
	Random r;

	//mouseX = 0.8f * mouseX + 0.2f * (getMouseXYRelative().x / (float)getWidth());
	float freq = 1.0f + 8.0f * std::min( 1.0f, std::max( 0.0f, (float)getMouseXYRelative().x / (float)getWidth() ) );
	if (p.onlyHarmonics.getToggleState()) {
		freq = ceil(freq);
	}
	p.frequency = freq;

	p.generateWaveform();

	// copy to fftbuffer
	for (int i = 0; i < p.fftBufferSize; i++) {
		if (i < p.fftSize) {
			//while (p.isGeneratingWaveform) {}
			p.fftBuffer[i] = p.saw[i];
		}
		else {
			// empty part of buffer for fft function to store complex numbers
			p.fftBuffer[i] = 0.0f;
		}
	}

	// perform fft
	p.fft->performRealOnlyForwardTransform(p.fftBuffer, true);

	// POWER MAGNITUDES
		// get power magnitudes
		for (int i = 0; i < p.fftSize; i++) {
			p.sawMags[i] = pow(p.fftBuffer[i * 2], 2.0f) + pow(p.fftBuffer[i * 2 + 1], 2.0f);
		}

		// Get powermagnitudes for drawing
			// copy to drawmags
			for (int i = 0; i < p.fftSize; i++) {
				p.drawMags[i] = p.sawMags[i];
			}
			// find max powermagnitude
			float max = 0.0f;
			for (int i = 0; i < p.fftSize; i++) {
				if (p.drawMags[i] > max)
					max = p.drawMags[i];
			}
			//DBG(max);
			// normalize with max value or normalizationfactor
			for (int i = 0; i < p.fftSize; i++) {
				//sawMags[i] /= (float)normalizationFactor;
				p.drawMags[i] /= max;
			}
			// clip to range or check range
			for (int i = 0; i < p.fftSize; i++) {
				p.drawMags[i] = std::min(1.0f, std::max(0.0f, p.drawMags[i]));
				//jassert(saw[i] <= 1.0f);
				//jassert(saw[i] >= 0.0f);
			}
		
	// PHASES
	// get phase
	for (int i = 0; i < p.fftSize; i++) {
		p.sawPhas[i] = std::atan2(p.fftBuffer[i * 2 + 1], p.fftBuffer[i * 2]);
	}
	// shift phases
	mouseY = 0.8f * mouseY + 0.2f * std::min(1.0f, std::max(0.0f, getMouseXYRelative().y / (float)getHeight()));
	const float shift = MathConstants<float>::twoPi * mouseY;
	for (int i = 0; i < p.fftSize; i++) {
		p.sawPhas[i] += shift;
		p.sawPhas[i] = p.sawPhas[i] > MathConstants<float>::pi ? p.sawPhas[i] - MathConstants<float>::twoPi : p.sawPhas[i];
	}

		// normalize for drawing
		for (int i = 0; i < p.fftSize; i++) {
			p.drawPhas[i] = (p.sawPhas[i] + MathConstants<float>::pi) / MathConstants<float>::twoPi;
		}

	// reobtain cartesian coordinates
	for (int i = 0; i < p.fftSize; i++) {
		p.sawMags[i] = sqrt(p.sawMags[i]); // powermagnitudes to normal magnitudes
		p.fftBuffer[i * 2] = p.sawMags[i] * cos(p.sawPhas[i]);
		p.fftBuffer[i * 2 + 1] = p.sawMags[i] * sin(p.sawPhas[i]);
	}
	// perform ifft
	p.fft->performRealOnlyInverseTransform(p.fftBuffer);
	float dcoffset = 0.0f;
	for (int i = 0; i < p.fftSize; i++) {
		dcoffset += p.fftBuffer[i];
	}
	dcoffset /= (float)p.fftSize;
	for (int i = 0; i < p.fftSize; i++) {
		p.audioBuffer[i] = p.fftBuffer[i] - dcoffset;
	}

	/*********** DRAWING ************/

	const float rf = 7.5f;
	auto drawSpace = getLocalBounds();
	auto header = drawSpace.removeFromTop( 50 );

	const auto waveformS = drawSpace.removeFromLeft(drawSpace.getHeight()).reduced(rf, rf);
	const auto wet_waveFS = drawSpace.removeFromRight(drawSpace.getHeight()).reduced(rf, rf);

	const auto magSpace = drawSpace.removeFromTop(drawSpace.getHeight() / 2).reduced(rf, rf);
	const auto phaSpace = drawSpace.reduced(rf, rf);

	g.setColour(Colours::black);
	g.fillRect(magSpace);
	g.fillRect(phaSpace);
	g.fillRect(waveformS);
	g.fillRect(wet_waveFS);

	// draw magnitude and phase
	const float spacePerBin = magSpace.getWidth() / (0.5f*(float)p.fftSize);
	const float y = (float)magSpace.getTopLeft().y;
	for (int i = 0; i < p.fftSize / 2; i++) {
		g.setColour(Colours::aqua);
		g.fillRect(magSpace.getTopLeft().x + i * spacePerBin, y, ceil(spacePerBin), (1.0f - p.drawMags[i]) * magSpace.getHeight());
		g.setColour(Colours::aqua.darker( 2.0f * pow(1.0f - p.drawMags[i], 0.2f) ) );
		g.fillRect(phaSpace.getTopLeft().x + i * spacePerBin, phaSpace.getTopLeft().y + (1.0f - p.drawPhas[i]) * phaSpace.getHeight(), spacePerBin, 1.5f + p.drawMags[i]*5.0f);
	}

	// draw dry waveform
	g.setColour(Colours::aqua);
	const float spacePerAmp = waveformS.getWidth() / (float)p.fftSize;
	Path pa;
	pa.startNewSubPath( waveformS.getTopLeft().x, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.saw[0])) * waveformS.getHeight() );
	for (int i = 0; i < p.fftSize; i++) {
		g.fillRect(waveformS.getTopLeft().x + i * spacePerAmp, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.saw[i]) ) * waveformS.getHeight(), spacePerAmp, 5.0f);
		pa.lineTo(waveformS.getTopLeft().x + i * spacePerAmp, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.saw[i])) * waveformS.getHeight());
	}
	g.strokePath(pa, PathStrokeType(2.0f));

	// draw wet waveform
	g.setColour(Colours::aqua);
	Path p2;
	p2.startNewSubPath( wet_waveFS.getTopLeft().x, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.fftBuffer[0])) * wet_waveFS.getHeight() );
	for (int i = 0; i < p.fftSize; i++) {
		g.fillRect(wet_waveFS.getTopLeft().x + i * spacePerAmp, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.fftBuffer[i])) * wet_waveFS.getHeight(), spacePerAmp, 5.0f);
		p2.lineTo(wet_waveFS.getTopLeft().x + i * spacePerAmp, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * p.fftBuffer[i])) * wet_waveFS.getHeight());
	}
	g.strokePath(p2, PathStrokeType(2.0f));

	g.setColour(Colours::aqua.darker(1.0f).darker(0.7f));
	g.fillRect(header);
	g.setColour(Colours::aqua.darker(0.4f));
	g.setFont( std::max( 10.0f, getHeight() * 0.023f ) );
	g.drawFittedText("This program makes it possible to visualize the influence of FFT phase rotation on sawtooth signals.\n\thorizontal mouse movements lets control the generator's frequency\n\tvertical mouse movements perform a phase rotation as can be seen in the fft phase output", header.withTrimmedRight(0.3f * getWidth()).reduced(5.0f), Justification::topLeft, 3);
	g.setFont(12.0f);
	g.setColour(Colours::aqua.darker(0.4f).withAlpha(0.5f));
	g.drawFittedText("Created by Harold Groenenboom", wet_waveFS, Justification::bottomRight, 1);
}

void FftphaseRotationAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto drawSpace = getLocalBounds();
	auto header = drawSpace.removeFromTop(50);

	p.onlyHarmonics.setBounds(header.removeFromRight(header.getHeight()*2.0f));

	// RIGHT HEADER
	auto rightheader = header.removeFromRight(0.4f * getWidth());
	{
		rightheader.removeFromRight(0.02f * getWidth());

		auto comboboxspace = rightheader.removeFromRight(header.getHeight()*2.0f);
		p.fftSizeSetting.setBounds(comboboxspace.removeFromBottom(0.5f * rightheader.getHeight()));
		combobox_t.setBoundingBox(Parallelogram<float>(comboboxspace.toFloat()));

		rightheader.removeFromRight(0.02f * getWidth());

		auto combobox2space = rightheader.removeFromRight(header.getHeight()*2.0f);
		p.waveformSetting.setBounds(combobox2space.removeFromBottom(0.5f * rightheader.getHeight()));
		combobox2_t.setBoundingBox(Parallelogram<float>(combobox2space.toFloat()));
	}
	resizableCorner->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
}

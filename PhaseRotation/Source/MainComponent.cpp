/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include <algorithm>

//==============================================================================
MainComponent::MainComponent()
	: fft(fftOrder)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1300, 430);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

	startTimerHz(60);
	addAndMakeVisible(&onlyHarmonics);
	onlyHarmonics.setButtonText("Only\nharmonics");
	onlyHarmonics.setColour( onlyHarmonics.textColourId, Colours::aqua.darker(0.4f) );
	onlyHarmonics.setColour( onlyHarmonics.tickColourId, Colours::aqua.darker(0.4f) );
	onlyHarmonics.setColour( onlyHarmonics.tickDisabledColourId, Colours::aqua.darker(0.4f) );
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

int counter = 0;
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
	auto b = bufferToFill.buffer;
	auto buffer = b->getArrayOfWritePointers();

	for (int i = 0; i < b->getNumSamples(); i++) {
		for (int c = 0; c < b->getNumChannels(); c++) {
			buffer[c][i] = audioBuffer[counter];
			counter++;
			counter %= fftSize;
		}
	}
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
float mouseX = 0.0f;
float mouseY = 0.0f;
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
	Random r;

	mouseX = 0.8f * mouseX + 0.2f * (getMouseXYRelative().x / (float)getWidth());
	float freq = 1.0f + 0.5f * 16.0f * std::min( 1.0f, std::max( 0.0f, mouseX ) );
	if (onlyHarmonics.getToggleState()) {
		freq = ceil(freq);
	}
	//DBG(std::to_string(freq));

	// generate waveform
	for (int i = 0; i < fftSize; i++) {
		//saw[i] = 1.0f - 2.0f * std::fmod(freq * i / (float)fftSize, 1.0f); //saw
		//saw[i] = r.nextFloat(); //noise (0-1)
		//saw[i] = 1.0f - 2.0f*r.nextFloat(); // noise (-1 - 1)
		saw[i] = std::sin(MathConstants<float>::twoPi * 2.0f * (i / (float)fftSize) * freq); // sin
		saw[i] *= 0.5f;
	}

	// copy to fftbuffer
	for (int i = 0; i < fftBufferSize; i++) {
		if (i < fftSize) {
			fftBuffer[i] = saw[i];
		}
		else {
			// empty part of buffer for fft function to store complex numbers
			fftBuffer[i] = 0.0f;
		}
	}

	// perform fft
	fft.performRealOnlyForwardTransform(fftBuffer, true);

	// POWER MAGNITUDES
		// get power magnitudes
		for (int i = 0; i < fftSize; i++) {
			sawMags[i] = pow(fftBuffer[i * 2], 2.0f) + pow(fftBuffer[i * 2 + 1], 2.0f);
		}

		// Get powermagnitudes for drawing
			// copy to drawmags
			for (int i = 0; i < fftSize; i++) {
				drawMags[i] = sawMags[i];
			}
			// find max powermagnitude
			float max = 0.0f;
			for (int i = 0; i < fftSize; i++) {
				if (drawMags[i] > max)
					max = drawMags[i];
			}
			//DBG(max);
			// normalize with max value or normalizationfactor
			for (int i = 0; i < fftSize; i++) {
				//sawMags[i] /= (float)normalizationFactor;
				drawMags[i] /= max;
			}
			// clip to range or check range
			for (int i = 0; i < fftSize; i++) {
				drawMags[i] = std::min(1.0f, std::max(0.0f, drawMags[i]));
				//jassert(saw[i] <= 1.0f);
				//jassert(saw[i] >= 0.0f);
			}
		
	// PHASES
	// get phase
	for (int i = 0; i < fftSize; i++) {
		sawPhas[i] = std::atan2(fftBuffer[i * 2 + 1], fftBuffer[i * 2]);
	}
	// shift phases
	mouseY = 0.8f * mouseY + 0.2f * std::min(1.0f, std::max(0.0f, getMouseXYRelative().y / (float)getHeight()));
	const float shift = MathConstants<float>::twoPi * mouseY;
	for (int i = 0; i < fftSize; i++) {
		sawPhas[i] += shift;
		sawPhas[i] = sawPhas[i] > MathConstants<float>::pi ? sawPhas[i] - MathConstants<float>::twoPi : sawPhas[i];
	}

		// normalize for drawing
		for (int i = 0; i < fftSize; i++) {
			drawPhas[i] = (sawPhas[i] + MathConstants<float>::pi) / MathConstants<float>::twoPi;
		}

	// reobtain cartesian coordinates
	for (int i = 0; i < fftSize; i++) {
		sawMags[i] = sqrt(sawMags[i]); // powermagnitudes to normal magnitudes
		fftBuffer[i * 2] = sawMags[i] * cos(sawPhas[i]);
		fftBuffer[i * 2 + 1] = sawMags[i] * sin(sawPhas[i]);
	}
	// perform ifft
	fft.performRealOnlyInverseTransform(fftBuffer);
	float dcoffset = 0.0f;
	for (int i = 0; i < fftSize; i++) {
		dcoffset += fftBuffer[i];
	}
	dcoffset /= (float)fftSize;
	for (int i = 0; i < fftSize; i++) {
		audioBuffer[i] = fftBuffer[i] - dcoffset;
	}

	/*********** DRAWING ************/

	auto drawSpace = getLocalBounds();
	auto header = drawSpace.removeFromTop( 50 );

	auto waveformS = drawSpace.removeFromLeft(drawSpace.getHeight());
	auto wet_waveFS = drawSpace.removeFromRight(drawSpace.getHeight());

	auto magSpace = drawSpace.removeFromTop(drawSpace.getHeight() / 2);
	auto phaSpace = drawSpace; //magSpace.removeFromBottom();

	const float rf = 7.5f;
	waveformS.reduce(rf, rf);
	wet_waveFS.reduce(rf, rf);
	magSpace.reduce(rf, rf);
	phaSpace.reduce(rf, rf);
	
	g.setColour(Colours::black);
	g.fillRect(magSpace);
	g.fillRect(phaSpace);
	g.fillRect(waveformS);
	g.fillRect(wet_waveFS);

	// draw magnitude and phase
	const float spacePerBin = magSpace.getWidth() / (0.5f*(float)fftSize);
	for (int i = 0; i < fftSize / 2; i++) {
		g.setColour(Colours::aqua);
		g.fillRect(magSpace.getTopLeft().x + i * spacePerBin, (float)magSpace.getTopLeft().y, ceil(spacePerBin), (1.0f - drawMags[i]) * magSpace.getHeight());
		g.setColour(Colours::aqua.darker( sqrt(1.0f - drawMags[i])) );
		g.fillRect(phaSpace.getTopLeft().x + i * spacePerBin, phaSpace.getTopLeft().y + (1.0f - drawPhas[i]) * phaSpace.getHeight(), spacePerBin, 1.5f+ drawMags[i]*5.0f);
	}

	// draw dry waveform
	g.setColour(Colours::aqua);
	const float spacePerAmp = waveformS.getWidth() / (float)fftSize;
	Path p;
	p.startNewSubPath( waveformS.getTopLeft().x, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * saw[0])) * waveformS.getHeight() );
	for (int i = 0; i < fftSize; i++) {
		g.fillRect(waveformS.getTopLeft().x + i * spacePerAmp, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * saw[i]) ) * waveformS.getHeight(), spacePerAmp, 5.0f);
		p.lineTo(waveformS.getTopLeft().x + i * spacePerAmp, waveformS.getTopLeft().y + (1.0f - (0.5f + 0.5f * saw[i])) * waveformS.getHeight());
	}
	g.strokePath(p, PathStrokeType(2.0f));

	// draw wet waveform
	g.setColour(Colours::aqua);
	Path p2;
	p2.startNewSubPath( wet_waveFS.getTopLeft().x, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * fftBuffer[0])) * wet_waveFS.getHeight() );
	for (int i = 0; i < fftSize; i++) {
		g.fillRect(wet_waveFS.getTopLeft().x + i * spacePerAmp, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * fftBuffer[i])) * wet_waveFS.getHeight(), spacePerAmp, 5.0f);
		p2.lineTo(wet_waveFS.getTopLeft().x + i * spacePerAmp, wet_waveFS.getTopLeft().y + (1.0f - (0.5f + 0.5f * fftBuffer[i])) * wet_waveFS.getHeight());
	}
	g.strokePath(p2, PathStrokeType(2.0f));

	g.setColour(Colours::aqua.darker(1.0f).darker(0.7f));
	g.fillRect(header);
	g.setColour(Colours::aqua.darker(0.4f));
	g.drawFittedText("This program makes it possible to visualize the influence of FFT phase rotation on sawtooth signals.\n\thorizontal mouse movements lets control the generator's frequency\n\tvertical mouse movements perform a phase rotation as can be seen in the fft phase output", header.withTrimmedRight(0.5f * getWidth()).reduced(5.0f), Justification::topLeft, 3);
	g.setFont(12.0f);
	g.setColour(Colours::aqua.darker(0.4f).withAlpha(0.5f));
	g.drawFittedText("Created by Harold Groenenboom", wet_waveFS, Justification::bottomRight, 1);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	auto drawSpace = getLocalBounds();
	auto header = drawSpace.removeFromTop(50);

	onlyHarmonics.setBounds(header.removeFromRight(header.getHeight()*2.5f).removeFromLeft(2.0f*header.getHeight()));
}

/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftphaseRotationAudioProcessor::FftphaseRotationAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	lookandfeel.setColour(ComboBox::focusedOutlineColourId, Colours::aqua);
	lookandfeel.setColour(ComboBox::outlineColourId, Colours::aqua);
	lookandfeel.setColour(ComboBox::textColourId, Colours::aqua.darker(0.4f));
	lookandfeel.setColour(ComboBox::arrowColourId, Colours::aqua.darker(0.4f));
	lookandfeel.setColour(ComboBox::buttonColourId, Colours::aqua.darker(0.4f));
	lookandfeel.setColour(ComboBox::backgroundColourId, Colours::aqua.withMultipliedSaturation(0.4f).darker(4));
	lookandfeel.setColour(PopupMenu::backgroundColourId, Colours::aqua.withMultipliedSaturation(0.4f).darker(4));
	lookandfeel.setColour(PopupMenu::headerTextColourId, Colours::aqua.darker(0.4f));
	lookandfeel.setColour(PopupMenu::highlightedBackgroundColourId, Colours::aqua.darker(0.4f));
	lookandfeel.setColour(PopupMenu::highlightedTextColourId, Colours::aqua.darker(0.1f));
	lookandfeel.setColour(PopupMenu::textColourId, Colours::aqua.darker(0.4f));

	onlyHarmonics.setButtonText("Only\nharmonics");
	onlyHarmonics.setColour(onlyHarmonics.textColourId, Colours::aqua.darker(0.4f));
	onlyHarmonics.setColour(onlyHarmonics.tickColourId, Colours::aqua.darker(0.4f));
	onlyHarmonics.setColour(onlyHarmonics.tickDisabledColourId, Colours::aqua.darker(0.4f));

	for (int i = 5; i < 11; i++) {
		fftSizeSetting.addItem(std::to_string(1 << i), i);
	}
	fftSizeSetting.onChange = [this] { fftSizeSettingCallback(); };
	fftSizeSetting.setSelectedId(7);
	fftSizeSettingCallback();
	fftSizeSetting.setLookAndFeel(&lookandfeel);

	waveformSetting.addItem("Saw", 1);
	waveformSetting.addItem("Sine", 2);
	waveformSetting.addItem("Noise", 3);
	waveformSetting.addItem("Soft Saw", 4);
	waveformSetting.addItem("Square", 5);
	waveformSetting.addItem("Triangle (Soft Square)", 6);
	waveformSetting.addItem("Soft Square", 7);
	waveformSetting.setSelectedId(4);
	waveformSetting.setLookAndFeel(&lookandfeel);

	smoothFrequency.setSmoothTime(0.005f * getSampleRate());
}

FftphaseRotationAudioProcessor::~FftphaseRotationAudioProcessor()
{
	if (fft != nullptr)
		delete fft;
}

//==============================================================================
const String FftphaseRotationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FftphaseRotationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FftphaseRotationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FftphaseRotationAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FftphaseRotationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FftphaseRotationAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FftphaseRotationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FftphaseRotationAudioProcessor::setCurrentProgram (int index)
{
}

const String FftphaseRotationAudioProcessor::getProgramName (int index)
{
    return {};
}

void FftphaseRotationAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FftphaseRotationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FftphaseRotationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftphaseRotationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

int counter = 0;
void FftphaseRotationAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	smoothFrequency = frequency;

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	auto b = &buffer;
	auto buf = b->getArrayOfWritePointers();

	for (int i = 0; i < b->getNumSamples(); i++) {
		for (int c = 0; c < b->getNumChannels(); c++) {
			buf[c][i] = 0.3f * readAudioBufferInterpolated((float)counter * invplaybackOscillationTime);
			counter++;
			counter %= playbackOscillationTime;
		}

		smoothFrequency.tick();
	}
}

//==============================================================================
bool FftphaseRotationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FftphaseRotationAudioProcessor::createEditor()
{
    return new FftphaseRotationAudioProcessorEditor (*this);
}

//==============================================================================
void FftphaseRotationAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FftphaseRotationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftphaseRotationAudioProcessor();
}

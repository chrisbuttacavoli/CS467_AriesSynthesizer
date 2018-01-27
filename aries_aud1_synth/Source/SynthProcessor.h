/*
  ==============================================================================

    SynthProcessor.h
    Created: 21 Jan 2018 9:55:11pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "OscillatorVoice.h"
#include "GenericEditor.h"
#include "maximilian.h"


//==============================================================================
/*
*/
class SynthProcessor : public AudioProcessor
{
public:
	SynthProcessor(MidiKeyboardState &keyState) : keyboardState(keyState) {

		// Initialize GUI controlled parameters
		//addParameter(gainParam = new AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.9f));

		// You can swap these lines to get one of these parameters to show up on the screen,
		// need to fix the GUI so that more than one shows
		addParameter(envReleaseParam = new AudioParameterFloat("release",
			"Release", 0.0f, 10.0f, 2.0f));
		addParameter(levelParam = new AudioParameterFloat("sineLevel",
			"Sine Level", 0.0f, 1.0f, 1.0f));
		addParameter(levelParam = new AudioParameterFloat("squareLevel",
			"Square Level", 0.0f, 1.0f, 0.2f));
		addParameter(levelParam = new AudioParameterFloat("sawLevel",
			"Saw Level", 0.0f, 1.0f, 0.0f));
		addParameter(levelParam = new AudioParameterFloat("noiseLevel",
			"Noise Level", 0.0f, 1.0f, 0.0f));
		addParameter(distAmountParam = new AudioParameterFloat("distAmount",
			"Distortion", 0.0f, 15.0f, 0.0f));

		mySynth.clearSounds();
		mySynth.addVoice(new OscillatorVoice());
		mySynth.addSound(new SynthSound());
	}

	~SynthProcessor() {	}

	// TODO: simplify this function
	const String getName() const override {
		return String("My Synth Processor");
	}

	void prepareToPlay(double sampleRate, int MaxSamplesPerBlock) override{
		midiCollector.reset(sampleRate);
		mySynth.setCurrentPlaybackSampleRate(sampleRate);
	}
	
	void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {
		// Hooking up our OscillatorVoice to our parameters.
		const OwnedArray<AudioProcessorParameter>& params = getParameters();
		if (myVoice = dynamic_cast<OscillatorVoice*>(mySynth.getVoice(0))) {
			myVoice->getParamsFromProcessor(params);
		}
		
		// the synth always adds its output to the audio buffer, so we have to clear it first..
		buffer.clear();

		// fill a midi buffer with incoming messages from the midi input.
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, buffer.getNumSamples());

		// pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.
		// This call will also add midi messages to the buffer which were generated by
		// the mouse-clicking on the on-screen keyboard.
		keyboardState.processNextMidiBuffer(incomingMidi, 0, buffer.getNumSamples(), true);

		// and now get the synth to process the midi events and generate its output.
		mySynth.renderNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());
	}

	void releaseResources() override {

	}

	double getTailLengthSeconds() const override {
		return 0;
	}

	bool acceptsMidi() const override {
		return true;
	}

	bool producesMidi() const override {
		return true;
	}


	/*************
	** Create Process Editor Class to generete new UI elements
	**
	*************/
	AudioProcessorEditor* createEditor() override{
		// Connects a processor editor to this processor
		return new GenericEditor (*this);
	}

	bool hasEditor() const override {
		return true;
	}

	int getNumPrograms() override {
		//temp?
		return 0;
	}

	void changeProgramName(int index, const String &newName) override {
		
	}

	int getCurrentProgram() override {
		//temp?
		return 0;
	}

	void setCurrentProgram(int index) override {
		
	}

	//derp
	const String getProgramName(int index) override {
		return "The Synth";
	}

	void setStateInformation(const void * data, int sizeInBytes) override {

	}

	void getStateInformation(juce::MemoryBlock &destData) override {

	}

	//Getting midi/keyboard messages
	MidiMessageCollector midiCollector;
	MidiKeyboardState& keyboardState;

	//the actual synth object
	Synthesiser mySynth;
	OscillatorVoice* myVoice;

	// Our parameters
	//AudioParameterFloat* gainParam = nullptr;
	AudioParameterFloat* envReleaseParam = nullptr;
	AudioParameterFloat* distAmountParam = nullptr;
	AudioParameterFloat* levelParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthProcessor)
};

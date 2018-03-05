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
#include "GenericEditor.h"
#include "maximilian.h"
#include "PatchManager.h"
#include "OscillatorVoice.h"

//==============================================================================
/*
*/
class SynthProcessor : public AudioProcessor
{
public:
	SynthProcessor(MidiKeyboardState &keyState) : keyboardState(keyState) {
		InitSynth();
		InitParameters();
		addParametersToMap();
	}

	void InitParameters()
	{
		/*
			Oscillator parameters
		*/
		const StringArray & oscillatorChoices = { "None", "Sine", "Saw", "Square", "Noise" };
		addParameter(new AudioParameterChoice("Oscillator1", "Oscillator1", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterChoice("Oscillator2", "Oscillator2", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterChoice("Oscillator3", "Oscillator3", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterChoice("Oscillator4", "Oscillator4", oscillatorChoices, noWave, "Cats"));

		addParameter(new AudioParameterFloat("Pitch1", "Pitch1", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("Level1", "Level1", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("Pitch2", "Pitch2", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("Level2", "Level2", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("Pitch3", "Pitch3", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("Level3", "Level3", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("Pitch4", "Pitch4", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("Level4", "Level4", 0.0f, 1.0f, 0.5f));

		/*
			Envelope parameters: The scale parameters act as multiplies since our values are passed
			are 0.0 to 1.0 in OscillatorVoice. To get a max of 10 seconds, we need to have a scale.
			Sustain units are not ms, they are level (0-100%).
		*/

		addScale("Attack", 10.0f);
		addScale("Decay", 10.0f);
		addScale("Release", 10.0f);
		addParameter(new AudioParameterFloat("Attack", "Attack", 0.0f, getScale("Attack"), 0.15f));
		addParameter(new AudioParameterFloat("Decay", "Decay", 0.0f, getScale("Decay"), 0.5f));
		addParameter(new AudioParameterFloat("Sustain", "Sustain", 0.0f, 1.0f, 1.0f));
		addParameter(new AudioParameterFloat("Release", "Release", 0.0f, getScale("Release"), 0.15f));


		/*
			Filter parameters
		*/
		const StringArray & filterChoices = { "None", "LoPass", "HiPass", "BandPass" };
		addParameter(new AudioParameterChoice("Filter", "Filter", filterChoices, noFilter, "Cats"));

		addScale("Cutoff", 14000); // The frequency
		addScale("Resonance", 10); // The boost
		addParameter(new AudioParameterFloat("Cutoff", "Cutoff", 1.0f, getScale("Cutoff"), 5000.0f));
		addParameter(new AudioParameterFloat("Resonance", "Resonance", 0.0f, getScale("Resonance"), getScale("Resonance")));


		/*
			LFO parameters
		*/
		addScale("LFOFreq", 10);
		addParameter(new AudioParameterFloat("LFOFreq", "LFOFreq", 1.0f, getScale("LFOFreq"), 2.0f));
		addParameter(new AudioParameterChoice("LFOosc", "LFOosc", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterFloat("LFOLevel", "LFOLevel", 0.0f, 1.0f, 0.5f));


		/*
			Distortion
		*/
		addScale("Distortion", 30.0f);
		addParameter(new AudioParameterFloat("Distortion", "Distortion", 0.0f, getScale("Distortion"), 0.0f));


		/*
			EQ parameters
		*/
		addScale("EQLevel", 10.0f);
		addScale("EQQ", 5.0f);

		addParameter(new AudioParameterFloat("EQLowFreq", "EQLowFreq", 20.0f, 800.0f, 125.0f));
		addParameter(new AudioParameterFloat("EQLowQ", "EQLowQ", 0.05f, getScale("EQQ"), 1.0f));
		addParameter(new AudioParameterFloat("EQLowLevel", "EQLowLevel", -getScale("EQLevel"), getScale("EQLevel"), 0.0f));

		addParameter(new AudioParameterFloat("EQMidFreq", "EQMidFreq", 250.0f, 4000.0f, 2500.0f));
		addParameter(new AudioParameterFloat("EQMidQ", "EQMidQ", 0.05f, getScale("EQQ"), 1.0f));
		addParameter(new AudioParameterFloat("EQMidLevel", "EQMidLevel", -getScale("EQLevel"), getScale("EQLevel"), 0.0f));

		addParameter(new AudioParameterFloat("EQHiFreq", "EQHiFreq", 800.0f, 20000.0f, 8000.0f));
		addParameter(new AudioParameterFloat("EQHiQ", "EQHiQ", 0.05f, getScale("EQQ"), 1.0f));
		addParameter(new AudioParameterFloat("EQHiLevel", "EQHiLevel", -getScale("EQLevel"), getScale("EQLevel"), 0.0f));


		/*
			Master Volume
		*/
		addScale("Master", 1.5f);
		addParameter(new AudioParameterFloat("Master", "Master", 0.0f, getScale("Master"), 1.0f));
	}

	void InitSynth()
	{
		// Dealing with the oscillators
		mySynth.clearVoices();

		// Use this loop to add multiple voices so that we can play more than one note
		for (int i = 0; i < 10; i++) {
			mySynth.addVoice(new OscillatorVoice());
		}
		mySynth.clearSounds();
		mySynth.addSound(new SynthSound());
	}

	~SynthProcessor() {	}

	// Add parameters to a dictionary to pass to OscillatorVoice
	void addParametersToMap() {
		const OwnedArray<AudioProcessorParameter>& params = getParameters();
		for (AudioProcessorParameter** ptr = params.begin(); ptr < params.end(); ptr++)
			paramMap.insert(std::pair<juce::String, AudioProcessorParameter*>((**ptr).getName(32), *ptr));
	}

	void addScale(juce::String key, float scale) {
		paramScaleMap.insert(pair<juce::String, float>(key, scale));
	}

	float getScale(juce::String key) {
		return paramScaleMap.at(key);
	}

	// TODO: simplify this function
	const String getName() const override {
		return String("My Synth Processor");
	}

	void prepareToPlay(double sampleRate, int MaxSamplesPerBlock) override{
		midiCollector.reset(sampleRate);
		mySynth.setCurrentPlaybackSampleRate(sampleRate);
		settings.setup(sampleRate, 2, MaxSamplesPerBlock);
		
		for (int i = 0; i < mySynth.getNumVoices(); i++)
		{
			mySynth.getVoice(i)->setCurrentPlaybackSampleRate(sampleRate);
		}
	}
	
	void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {
		// Hooking up our OscillatorVoice to our parameters.
		const OwnedArray<AudioProcessorParameter>& params = getParameters();
		for (int i = 0; i < mySynth.getNumVoices(); i++) {
			if (myVoice = dynamic_cast<OscillatorVoice*>(mySynth.getVoice(i))) {
				myVoice->getParamsFromProcessor(paramMap, paramScaleMap);
			}
		}

		// fill a midi buffer with incoming messages from the midi input.
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, buffer.getNumSamples());

		// pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.
		// This call will also add midi messages to the buffer which were generated by
		// the mouse-clicking on the on-screen keyboard.
		keyboardState.processNextMidiBuffer(incomingMidi, 0, buffer.getNumSamples(), true);

		// the synth always adds its output to the audio buffer, so we have to clear it first..
		buffer.clear();

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

	void loadPatch() {
		FileChooser fileChooser("Select a patch to load", File::getSpecialLocation(File::userDocumentsDirectory), "*.pat");
		if (!fileChooser.browseForFileToOpen())
			return;

		File file(fileChooser.getResult());
		
		Array<float> patchValues = patchManager.GetParamsFromFile(file);
		for (int i = 0; i < patchValues.size(); i++)
		{
			AudioProcessorParameter* param = getParameters()[i];
			float patchValue = patchValues[i];
			param->setValue(patchValue);
		}
	}

	void savePatch() {
		FileChooser fileChooser("Save a patch", File::getSpecialLocation(File::userDocumentsDirectory), "*.pat");
		if (!fileChooser.browseForFileToSave(true))
			return;

		File file(fileChooser.getResult());
		patchManager.SaveParamsToFile(getParameters(), file);
	}

	// This functions aren't going to be used, but we have to override them
	void getStateInformation(juce::MemoryBlock & destData) override { }
	void setStateInformation(const void * data,	int	sizeInBytes) override { }
	
	PatchManager patchManager;

	//Getting midi/keyboard messages
	MidiMessageCollector midiCollector;
	MidiKeyboardState& keyboardState;

	//the actual synth object
	Synthesiser mySynth;
	OscillatorVoice* myVoice;
	maxiSettings settings;

	map <juce::String, AudioProcessorParameter*> paramMap;
	map <juce::String, float> paramScaleMap;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthProcessor)
};

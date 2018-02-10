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
#include "OscillatorVoice_Failure.h" // Just change the Oscillator voice of choice here


//==============================================================================
/*
*/
class SynthProcessor : public AudioProcessor
{
public:
	SynthProcessor(MidiKeyboardState &keyState) : keyboardState(keyState) {

		/*
			Oscillator parameters
		*/
		const StringArray & oscillatorChoices = {"None", "Sine", "Saw", "Square", "Noise"};
		addParameter(new AudioParameterChoice("oscillator1","Oscillator1", oscillatorChoices, sineWave, "Cats"));
		addParameter(new AudioParameterChoice("oscillator2", "Oscillator2", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterChoice("oscillator3", "Oscillator3", oscillatorChoices, noWave, "Cats"));
		addParameter(new AudioParameterChoice("oscillator4", "Oscillator4", oscillatorChoices, noWave, "Cats"));

		addParameter(new AudioParameterFloat("pitch1", "Pitch1", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("level1", "Level1", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("pitch2", "Pitch2", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("level2", "Level2", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("pitch3", "Pitch3", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("level3", "Level3", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("pitch4", "Pitch4", -1.0f, 1.0f, 0.0f));
		addParameter(new AudioParameterFloat("level4", "Level4", 0.0f, 1.0f, 0.5f));

		/* 
			Envelope parameters: The scale parameters act as multiplies since our values are passed
			are 0.0 to 1.0 in OscillatorVoice. To get a max of 10 seconds, we need to have a scale.
			Sustain units are not ms, they are level (0-100%).
		*/

		addScale("Attack", 10.0f);
		addScale("Decay", 10.0f);
		addScale("Release", 10.0f);
		addParameter(new AudioParameterFloat("attack", "Attack", 0.0f, getScale("Attack"), 3.0f));
		addParameter(new AudioParameterFloat("decay", "Decay", 0.0f, getScale("Decay"), 0.5f));
		addParameter(new AudioParameterFloat("sustain", "Sustain", 0.0f, 1.0f, 1.0f));
		addParameter(new AudioParameterFloat("release", "Release", 0.0f, getScale("Release"), 1.0f));

		/*
			Filter parameters
		*/
		const StringArray & filterChoices = { "None", "LoPass", "HiPass", "BandPass" };
		addParameter(new AudioParameterChoice("filter", "Filter", filterChoices, noFilter, "Cats"));

		addScale("Cutoff", 14000);
		addScale("Resonance", 200);
		addParameter(new AudioParameterFloat("cutoff", "Cutoff", 1.0f, getScale("Cutoff"), 5000.0f));
		addParameter(new AudioParameterFloat("resonance", "Resonance", 1.0f, getScale("Resonance"), 50.0f));


		/*
			LFO parameters
		*/
		addParameter(new AudioParameterChoice("lfoosc", "LFOosc", oscillatorChoices, sineWave, "Cats"));	//defaulted to sine

		//set to some default values before UI is added
		addScale("LFOFreq", 20);
		addParameter(new AudioParameterFloat("lfolevel", "LFOLevel", 0.0f, 1.0f, 0.5f));
		addParameter(new AudioParameterFloat("lfofreq", "LFOFreq", 1.0f, getScale("LFOFreq"), 10.0f));

		/*
			Other parameters
		*/
		addScale("Distortion", 30.0f);
		addParameter(new AudioParameterFloat("distAmount", "Distortion", 0.0f, getScale("Distortion"), 0.0f));

		addParametersToMap();
		
		//Dealing with the oscillators
		mySynth.clearVoices();

		// Use this loop to add multiple voices so that we can play more than one note
		for (int i = 0; i < 10; i++) {
			mySynth.addVoice(new OscillatorVoice());
		}
		mySynth.clearSounds();
		mySynth.addSound(new SynthSound());
	}

	~SynthProcessor() {	}

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
	maxiSettings settings;

	/*
		The paramScaleMap gives us multiplers for parameters when passing
		parameter values (0.0 to 1.0) to the OscillatorVoice. For instance,
		the attack may range from 0 to 15 seconds, so our multiplier would
		be 15 and would need to be considered in the envelope calculation.
	*/
	map <juce::String, AudioProcessorParameter*> paramMap;
	map <juce::String, float> paramScaleMap;

	// Filters
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> lowPassFilter;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> highPassFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthProcessor)
};

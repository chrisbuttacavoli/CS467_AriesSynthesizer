/*
  ==============================================================================

    OscillatorVoice.h
    
	Provides the wave data for any oscillator shape

  ==============================================================================
*/

#pragma once
//#include "../../maximillion/maximilian.h" // for alex compilation
#include "maximilian.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "Distortion.h"
#include "Envelope.h"
#include "Filter.h"
#include "LFO.h"
#include "Oscillator.h"
#include <map>


// This is used for debugging: DBG(FloatToStr(someFloatValue))
#include <sstream>
string FloatToStr(float val) {
	std::ostringstream ss;
	ss << val;
	std::string s(ss.str());
	return s;
}
string DoubleToStr(double val) {
	std::ostringstream ss;
	ss << val;
	std::string s(ss.str());
	return s;
}


class OscillatorVoice : public SynthesiserVoice {

public:
	OscillatorVoice() : keyPressed(0), osc1(OscillatorType::noWave),
		osc2(OscillatorType::noWave), osc3(OscillatorType::noWave),
		osc4(OscillatorType::noWave)
	{
	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);

		// Initializes pitches, it needs this to make sound
		osc1.initializePitch(frequency);
		osc2.initializePitch(frequency);
		osc3.initializePitch(frequency);
		osc4.initializePitch(frequency);

		env.startNote();

		keyPressed = 1;
	}

	// called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		clearCurrentNote();
		env.stopNote();
	}

	// called to let the voice know that the pitch wheel has been moved
	void pitchWheelMoved(int newPitchWheelValue) override {

	}

	// controller moved
	void controllerMoved(int controllerNumber, int newControllerValue) override {

	}

	// move to a listener class within the synthprocessor - Victoria
	void getParamsFromProcessor(map <juce::String, AudioProcessorParameter*> paramMap,
			map <juce::String, float> paramScaleMap) {
		osc1.setType(paramMap.at("Oscillator1")->getValue(), numOscillators);
		osc1.adjustPitch(paramMap.at("Pitch1")->getValue(), frequency);
		osc1.level = paramMap.at("Level1")->getValue();

		osc2.setType(paramMap.at("Oscillator2")->getValue(), numOscillators);
		osc2.adjustPitch(paramMap.at("Pitch2")->getValue(), frequency);
		osc2.level = paramMap.at("Level2")->getValue();

		osc3.setType(paramMap.at("Oscillator3")->getValue(), numOscillators);
		osc3.adjustPitch(paramMap.at("Pitch3")->getValue(), frequency);
		osc3.level = paramMap.at("Level3")->getValue();

		osc4.setType(paramMap.at("Oscillator4")->getValue(), numOscillators);
		osc4.adjustPitch(paramMap.at("Pitch4")->getValue(), frequency);
		osc4.level = paramMap.at("Level4")->getValue();

		env.setAttack(paramMap.at("Attack")->getValue() * 1000
				* paramScaleMap.at("Attack"));
		env.setDecay(paramMap.at("Decay")->getValue() * 1000
			* paramScaleMap.at("Decay"));
		env.setSustain(paramMap.at("Sustain")->getValue());
		env.setRelease(paramMap.at("Release")->getValue() * 1000
			* paramScaleMap.at("Release"));

		dist.setDistortion(paramMap.at("Distortion")->getValue() * paramScaleMap.at("Distortion"));

		filter.setType(paramMap.at("Filter")->getValue(), numFilterTypes);
		filter.setCutoffFreq(paramMap.at("Cutoff")->getValue() * paramScaleMap.at("Cutoff"));
		filter.setResonanceBoost(paramMap.at("Resonance")->getValue() * paramScaleMap.at("Resonance"));

		lfo.setOscType(paramMap.at("LFOosc")->getValue(), numOscillators);
		lfo.setOscFreq(paramMap.at("LFOFreq")->getValue() * paramScaleMap.at("LFOFreq"));
		lfo.setOscLevel(paramMap.at("LFOLevel")->getValue());
	}

	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		if (!keyPressed) return;
		
		for (int sample = 0; sample < numSamples; ++sample) {
			// Add all our oscillators together
			double wave = osc1.getWave() + osc2.getWave()
				+ osc3.getWave() + osc4.getWave();
			
			// Apply post process effects
			wave = applyEffects(outputBuffer, wave);

			// Output the final wave product
			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
				outputBuffer.addSample(channel, startSample, wave);
			}
			++startSample;
		}
	}

	float applyEffects(AudioBuffer<float> &buffer, double wave) {
		wave = dist.apply(wave);
		wave = filter.apply(wave);
		wave = lfo.apply(wave);	//must put LFO here to piggy back off of env trigger
		wave = env.apply(wave);

		return wave;
	}

private:
	int numOscillators = 4;
	int numFilterTypes = 4;
	double frequency;
	int keyPressed;

	//maxiEnv env;
	
	Oscillator osc1;
	Oscillator osc2;
	Oscillator osc3;
	Oscillator osc4;
	Distortion dist;
	Envelope env;
	Filter filter;
	LFO lfo;


	double loPassCutoff;
	double loPassRes;
};
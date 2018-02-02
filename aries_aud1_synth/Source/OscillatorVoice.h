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
	OscillatorVoice() : keyPressed(0), osc1(OscillatorType::none),
		osc2(OscillatorType::none), osc3(OscillatorType::none),
		osc4(OscillatorType::none)
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

		this->initEnvelope();
		keyPressed = 1;
	}

	// called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		clearCurrentNote();
		env.trigger = 0;
		//keyPressed = 0;
	}

	// called to let the voice know that the pitch wheel has been moved
	void pitchWheelMoved(int newPitchWheelValue) override {

	}

	// controller moved
	void controllerMoved(int controllerNumber, int newControllerValue) override {

	}

	void initEnvelope() {
		// Slightly raise the attack and release to get rid of clicks.
		// See for theory: https://www.youtube.com/watch?v=9niampRkFW0
		//env.setAttack(50);
		//env.setAttack(10000);
		/*env.setDecay(1);
		env.setSustain(50);
		env.setRelease(50);*/
		env.amplitude = 0.1;
		env.trigger = 1;
	}

	//move to a listener class within the synthprocessor - Victoria
	void getParamsFromProcessor(std::map <juce::String, AudioProcessorParameter*> paramMap) {
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

		distortionAmount = paramMap.at("Distortion")->getValue() + distortionMin;
		
		// +50ms min on some params, * 10 to match param knob
		if (paramMap.at("Attack")->getValue() < 0.05f) {
			env.setAttack(50);
		}
		else {
			env.setAttack((paramMap.at("Attack")->getValue() * 1000) * 10 + 50);
		}
		env.setDecay(paramMap.at("Decay")->getValue() * 1000); // This one is special I think?
		env.setSustain(paramMap.at("Sustain")->getValue() * 1000 * 10 + 50);
		env.setRelease(paramMap.at("Release")->getValue() * 1000 * 10 + 50);
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
		// Apply distortion
		wave = distortion.atanDist(wave, distortionAmount);
		
		// Apply an envelope
		wave = env.adsr(wave, env.trigger);

		return wave;
	}

private:
	int numOscillators = 4;
	double frequency;
	int keyPressed;

	maxiEnv env;
	maxiDistortion distortion;
	Oscillator osc1;
	Oscillator osc2;
	Oscillator osc3;
	Oscillator osc4;

	// Temporary private variables to hold param values for PoC
	double distortionAmount;
	double distortionMin = 0.1f;
};
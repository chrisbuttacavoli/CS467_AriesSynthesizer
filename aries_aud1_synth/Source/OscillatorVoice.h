/*
  ==============================================================================

    OscillatorVoice.h
    
	Provides the wave data for any oscillator shape

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Enums.h"
#include "maximilian.h"
//#include "../../maximillion/maximilian.h" // for alex compilation
#include "SynthSound.h"


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
	OscillatorVoice() : keyPressed(0)
	{
		// Initialize parameter values
		distortionMin = 0.1f;
	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
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
		env.setAttack(50);
		env.setDecay(1);
		env.setSustain(50);
		env.setRelease(50);
		env.amplitude = 0.1;
		env.trigger = 1;
	}

	void getParamsFromProcessor(const OwnedArray<AudioProcessorParameter>& params) {
		// Need to iterate over this array since there isn't a method to get by name/id etc
		for (AudioProcessorParameter** ptr = params.begin(); ptr < params.end(); ptr++)
		{
			/*
			// This is a cheesy way of doing this, will have to figure out a fater way like
			// using a dictionary to update the parameters. Parameter values are all between
			// 0.0 and 1.0, so we need to handle them on a parameter by parameter basis
			*/
			if ((**ptr).getName(32) == "Distortion")
				distortionAmount = (**ptr).getValue() + distortionMin;

			// Oscillator parameters
			else if ((**ptr).getName(32) == "Sine Level")
				this->sineLevel = (**ptr).getValue();

			else if ((**ptr).getName(32) == "Square Level")
				this->squareLevel = (**ptr).getValue();

			else if ((*ptr)->getName(32) == "Saw Level")
				this->sawLevel = (**ptr).getValue();

			else if ((*ptr)->getName(32) == "Noise Level")
				this->noiseLevel = (**ptr).getValue();

			// Envelope parameters
			else if ((**ptr).getName(32) == "Release") {
				// Value in GUI is displayed in seconds, but release needs ms, so 10*1000
				// Min value (behind the scecnes) is 50 ms to reduce clicks
				env.setRelease((**ptr).getValue() * 10000 + 50);
			}
		}
	}

	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		
		if (!keyPressed) return;
		
		for (int sample = 0; sample < numSamples; ++sample) {
			// Add all our oscillators together
			double wave = (sineOsc.sinewave(frequency) * sineLevel)
				+ (squareOsc.square(frequency) * squareLevel)
				+ (sawOsc.saw(frequency) * sawLevel)
				+ ((random.nextFloat() * 0.25f - 0.125f) * noiseLevel);

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
	double frequency;
	int keyPressed;

	// We need multiple maxiOsc's otherwise it is computationally inefficient and distorts
	maxiOsc sineOsc;
	maxiOsc squareOsc;
	maxiOsc sawOsc;
	maxiEnv env;
	maxiDistortion distortion;
	Random random; // For the noise oscillator

	// Temporary private variables to hold param values for PoC
	double sineLevel;
	double squareLevel;
	double sawLevel;
	double noiseLevel;
	double distortionAmount;
	double distortionMin;
};
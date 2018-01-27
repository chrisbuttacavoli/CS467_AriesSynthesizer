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


// This is used for debugging: DBG(ToString(someInt))
#include <sstream>
template <typename T>
string ToString(T val)
{
	stringstream stream;
	stream << val;
	return stream.str();
};
///////////////


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
	OscillatorVoice(OscillatorType oscType) : keyPressed(0)
	{
		this->oscType = oscType;
		distortionAmount = 0.1f;
		level = 1;
	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		//level = velocity;
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
		env.setAttack(1000);
		env.setDecay(1);
		env.setSustain(100);
		env.setRelease(1000);
		env.amplitude = 0.1;
		env.trigger = 1;
	}

	// Get updated values from the processor here
	void getParamsFromProcessor(float releaseParam, double distParam, float levelParam) {
		env.setRelease((double)releaseParam);
		
		if (distParam <= 0.05f)
			distParam = 0.0f;
		distortionAmount = 0.1f + distParam; // A value of 0 would mute everything, offset by a smidge

		level = levelParam;
	}

	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		
		if (!keyPressed) return;
		
		for (int sample = 0; sample < numSamples; ++sample) {
			
			// Get our basic wave shape based on the oscillator type
			double wave = getWave() * level;

			// Apply a basic envelope to get rid of clicks
			wave = applyEffects(outputBuffer, wave);

			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
				// Keep this loop clean, apply effects to the finalWave
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
	double level;
	double frequency;
	int keyPressed;
	OscillatorType oscType;
	maxiOsc osc;
	maxiEnv env;
	maxiDistortion distortion;
	Random random;

	// Temporary private variables to hold param values for PoC
	double distortionAmount;

	// This function outputs a wave form based on how the object was constructed
	double getWave() {
		switch (oscType)
		{
		case sineWave:
			return osc.sinewave(frequency);
			break;
		case sawWave:
			return osc.saw(frequency);
			break;
		case squareWave:
			return osc.square(frequency);
			break;
		case noiseWave:
			return random.nextFloat() * 0.25f - 0.125f;
			break;
		default:
			break;
		}
	}
};
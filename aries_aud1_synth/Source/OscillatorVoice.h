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
		env.setAttack(50);
		env.setDecay(1);
		env.setSustain(50);
		env.setRelease(50);
		env.amplitude = 0.1;
		env.trigger = 1;
	}

	//move to a listener class within the synthprocessor - Victoria
	void getParamsFromProcessor(const OwnedArray<AudioProcessorParameter>& params,
		std::map <juce::String, AudioProcessorParameter*> paramMap) {
		
		/*distortionAmount = paramMap.at("Distortion")->getValue() + distortionMin;
		sineOsc.level = paramMap.at("Sine Level")->getValue();
		squareOsc.level = paramMap.at("Square Level")->getValue();
		squareOsc.adjustPitch(paramMap.at("Square Pitch")->getValue(), frequency);
		sawOsc.level = paramMap.at("Saw Level")->getValue();
		env.setRelease(paramMap.at("Release")->getValue() * 10000 + 50);*/
		
		// Need to iterate over this array since there isn't a method to get by name/id etc
		for (AudioProcessorParameter** ptr = params.begin(); ptr < params.end(); ptr++)
		{
			/*
			// This is a cheesy way of doing this, will have to figure out a fater way like
			// using a dictionary to update the parameters. Parameter values are all between
			// 0.0 and 1.0, so we need to handle them on a parameter by parameter basis
			*/
			float val = (**ptr).getValue();
			juce::String paramName = (**ptr).getName(32);

			if (paramName == "Oscillator1") {
				osc1.type = static_cast<OscillatorType>(int(val * numOscillators + 0.5));
			}

			if (paramName == "Oscillator2") {
				osc2.type = static_cast<OscillatorType>(int(val * numOscillators + 0.5));
			}
			
			if (paramName == "Oscillator3") {
				osc3.type = static_cast<OscillatorType>(int(val * numOscillators + 0.5));
			}

			if (paramName == "Oscillator4") {
				osc4.type = static_cast<OscillatorType>(int(val * numOscillators + 0.5));
			}

			else if (paramName == "Pitch2") {
				//this->sineLevel = (**ptr).getValue();
				osc2.adjustPitch(val, frequency);
			}
			else if (paramName == "Level3") {
				//this->squareLevel = (**ptr).getValue();
				osc3.level = val;
			}
			else if (paramName == "Pitch3") {
				//this->sineLevel = (**ptr).getValue();
				osc3.adjustPitch(val, frequency);
			}
			else if (paramName == "Level4") {
				//this->squareLevel = (**ptr).getValue();
				osc4.level = val;
			}
			else if (paramName == "Pitch4") {
				//this->sineLevel = (**ptr).getValue();
				osc4.adjustPitch(val, frequency);
			}
			// Envelope parameters
			//1000 ms = 1 second
			else if (paramName == "Attack") {
				// Value in GUI is displayed in seconds, but release needs ms, so 10*1000
				// Min value (behind the scenes) is 50 ms to reduce clicks
				if (val < 0.05f) {
					env.setAttack(50);
				}
				env.setAttack(val * 1000);
			}
			else if (paramName == "Decay") {
				// Value in GUI is displayed in seconds, but release needs ms, so 10*1000
				env.setDecay(val * 1000);
			}
			else if (paramName == "Sustain") {
				// Value in GUI is displayed in seconds, but release needs ms, so 10*1000
				env.setSustain(val * 1000);
			}
			else if (paramName == "Release") {
				// Value in GUI is displayed in seconds, but release needs ms, so 10*1000
				env.setRelease(val * 1000);
			}
		}
	}

	// Maybe someone could figure out a more efficient algorithm?
	//double adjustFrequency(float octaveVal, double freq) {
	//	// Octave val is between 0.0 and 1.0
	//	return freq * pow(2, 2 * octaveVal - 1);
	//	//return freq * 2^(2*octaveVal - 1);
	//}

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
	double distortionMin;
};
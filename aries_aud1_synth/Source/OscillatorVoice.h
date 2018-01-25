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


class OscillatorVoice : public SynthesiserVoice {

public:

	OscillatorVoice(OscillatorType oscType) : level(0), keyPressed(0)
	{
		this->oscType = oscType;
	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		env.trigger = 1;
		level = velocity;
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);

		this->initEnvelope();

		keyPressed = 1;

		//print out midi note when its pressed
		std::cout << midiNoteNumber << std::endl;
	}

	// called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		clearCurrentNote();
		env.trigger = 0;
		keyPressed = 0;
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
		env.setSustain(100);
		env.setRelease(50);
		env.amplitude = 0.1;
	}

	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		// This function runs constantly, so return to make it super fast if we aren't hitting a note.
		// We can work on the tailoff feature later, but for now the frequency is actually working - Chris
		if (!keyPressed) return;
		
		for (int sample = 0; sample < numSamples; ++sample) {
			
			// Get our basic wave shape based on the oscillator type
			double wave = getWave();

			// Apply a basic envelope to get rid of clicks
		 	double finalWave = env.adsr(wave, env.trigger);

			//Applying distortion - Victoria
			//Commenting out for now so as not to mess with the env code in the loop below
			/*atan distortion, see http://www.musicdsp.org/showArchiveComment.php?ArchiveID=104*/
			/*shape from 1 (soft clipping) to infinity (hard clipping)*/
			//double distortedWave = distortion.atanDist(finalWave, 5);
		
			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
				outputBuffer.addSample(channel, startSample, env.adsr(random.nextFloat() * 0.25f - 0.125f, env.trigger));
				//outputBuffer.addSample(channel, startSample, distortedWave); //for distortion testing - Victoria

			}

			++startSample;
		}
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
			return osc.noise();
			break;
		default:
			break;
		}
	}
};
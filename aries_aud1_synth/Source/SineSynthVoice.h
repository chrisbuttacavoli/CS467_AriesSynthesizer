/*
  ==============================================================================

    SineSynthVoice.h
    Created: 19 Jan 2018 1:55:12pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
//#include "../../maximillion/maximilian.h"

class SineSynthVoice : public SynthesiserVoice {

public:

	SineSynthVoice() : currentAngle(0), angleDelta(0), level(0), tailOff(0)
	{

	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		/*level = velocity;
		//when we hit a key we want our oscillator to play at a frequency
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);*/

		currentAngle = 0.0;
		level = velocity * 0.15;
		tailOff = 0.0;

		//get the note number so that we can set the cycles
		double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		double cyclesPerSample = cyclesPerSecond / getSampleRate();

		angleDelta = cyclesPerSample * 2.0 * double_Pi;

		//print out midi note when its pressed
		std::cout << midiNoteNumber << std::endl;
	}
	//called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		if (velocity == 0) {
			clearCurrentNote();
		}

		if (allowTailOff)
		{
			// start a tail-off by setting this flag. The render callback will pick up on
			// this and do a fade out, calling clearCurrentNote() when it's finished.

			if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
								// stopNote method could be called more than once.
				tailOff = 1.0;
		}
		else
		{
			// we're being told to stop playing immediately, so reset everything..

			clearCurrentNote();
			angleDelta = 0.0;
		}
	}
	//called to let the voice know that the pitch wheel has been moved
	void pitchWheelMoved(int newPitchWheelValue) override {

	}
	//controller moved
	void controllerMoved(int controllerNumber, int newControllerValue) override {

	}
	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSample) override {
		/* can add effects here?
		//Adjusting ordering might be a bit more complicating
		//Maybe create a function called applyEffects and deal with ordering inside applyEffects
		double theWave = osc1.sinewave(440);

		//iterating through samples
		for (int sample = 0; sample < numSample; sample++) {
		//get the audio waveform from oscillator
		//double theWave = mySine * level;

		//iterating through channels
		for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {

		outputBuffer.addSample(channel, startSample, theWave);
		}
		++startSample;
		}*/

		//temp sine wave got code from: https://github.com/WeAreROLI/JUCE/blob/master/examples/Demo/Source/Demos/AudioSynthesiserDemo.cpp

		if (angleDelta != 0.0)
		{
			if (tailOff > 0)
			{
				while (--numSample >= 0)
				{
					const float currentSample = (float)(std::sin(currentAngle) * level * tailOff);

					for (int i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;

					tailOff *= 0.99;

					if (tailOff <= 0.005)
					{
						clearCurrentNote();

						angleDelta = 0.0;
						break;
					}
				}
			}
			else
			{
				while (--numSample >= 0)
				{
					const float currentSample = (float)(std::sin(currentAngle) * level);

					for (int i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;
				}
			}
		}
	}
private:
	double currentAngle, angleDelta, level, tailOff;
};
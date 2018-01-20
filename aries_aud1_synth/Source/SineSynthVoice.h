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
#include "maximilian.h"


#include <sstream>
template <typename T>
string ToString(T val)
{
	stringstream stream;
	stream << val;
	return stream.str();
};


class SineSynthVoice : public SynthesiserVoice {

public:

	SineSynthVoice() : level(0), tailOff(0), silent(1)
	{

	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		level = velocity;
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		tailOff = 0.0;
		silent = 0;
		
		//For the working sine wave - Victoria
		/*currentAngle = 0.0;
		level = velocity * 0.15;
		tailOff = 0.0;

		//get the note number so that we can set the cycles
		double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		double cyclesPerSample = cyclesPerSecond / getSampleRate();

		angleDelta = cyclesPerSample * 2.0 * double_Pi;*/

		//print out midi note when its pressed
		std::cout << midiNoteNumber << std::endl;
	}
	//called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		level = 0;
		clearCurrentNote();
		silent = 1;
		//if (velocity == 0) {
		//	clearCurrentNote();
		//}

		//if (allowTailOff)
		//{
		//	// start a tail-off by setting this flag. The render callback will pick up on
		//	// this and do a fade out, calling clearCurrentNote() when it's finished.

		//	if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
		//						// stopNote method could be called more than once.
		//		tailOff = 1.0;
		//}
		//else
		//{
		//	// we're being told to stop playing immediately, so reset everything..
		//	clearCurrentNote();
		//}
	}
	//called to let the voice know that the pitch wheel has been moved
	void pitchWheelMoved(int newPitchWheelValue) override {

	}
	//controller moved
	void controllerMoved(int controllerNumber, int newControllerValue) override {

	}
	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		// This function runs constantly, so return to make it super fast if we aren't hitting a note.
		// We can work on the tailoff feature later, but for now the frequency is actually working - Chris
		if (silent)
		{
			return;
		}

		for (int sample = 0; sample < numSamples; ++sample) {
			
			double theWave = osc1.sinewave(frequency) * level;
			
			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
				outputBuffer.addSample(channel, startSample, theWave);
			}
			++startSample;
		}
		////adding the tail off back to our code
		//if (tailOff > 0)
		//{
		//	//iterating through samples
		//	while (--numSample >= 0)
		//	{
		//		const float mySine = osc1.sinewave(frequency) * level * tailOff;

		//		//iterating through channels
		//		for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
		//			outputBuffer.addSample(channel, startSample, mySine);
		//		}
		//		++startSample;

		//		tailOff *= 0.99;

		//		if (tailOff <= 0.005)
		//		{
		//			clearCurrentNote();
		//			break;
		//		}
		//	}
		//}
		//else {
		//	//iterating through samples
		//	while (--numSample >= 0)
		//	{
		//		const float mySine = osc1.sinewave(frequency) * level * tailOff;

		//		//iterating through channels
		//		for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
		//			outputBuffer.addSample(channel, startSample, mySine);
		//		}
		//		++startSample;
		//	}
		//}
	}
private:
	double level, tailOff;
	double frequency;
	int silent;
	maxiOsc osc1;
};
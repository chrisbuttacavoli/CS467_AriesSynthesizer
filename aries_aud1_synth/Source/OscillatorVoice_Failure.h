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


class OscillatorFailureVoice : public SynthesiserVoice {

public:
	OscillatorFailureVoice() : osc1(noWave), osc2(noWave), osc3(noWave), osc4(noWave), lfo(noWave)
	{
		// Initialize the envelopes??
		for (int i = 0; i < waves.size(); i++) {
			/*waves[i] = 0;
			tailoffs[i].initializeForTesting();
			tailoffs[i].startNote();
			tailoffs[i].stopNote();*/
		}
	}

	//must return true if this voice object is capable of playing the given sound
	bool canPlaySound(SynthesiserSound *sound) override {
		//cast the sound to a synthSound, if the cast works, then return true and play sound
		return dynamic_cast<SynthSound*>(sound) != nullptr;
	}
	//called to start a new note
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
		if (neverPlayedANoteBefore)
			currentNoteForVoice = 0; // Initializes this number for the first run

		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);

		// At some point we need to have a separate frequency for each note so that when
		// we stop playing a note and switch to another the, the original note retains
		// its frequency.
		//freqs[currentNoteForVoice] = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		/*osc1.initializePitch(freqs[currentNoteForVoice]);
		osc2.initializePitch(freqs[currentNoteForVoice]);
		osc3.initializePitch(freqs[currentNoteForVoice]);
		osc4.initializePitch(freqs[currentNoteForVoice]);*/

		osc1.initializePitch(frequency);
		osc2.initializePitch(frequency);
		osc3.initializePitch(frequency);
		osc4.initializePitch(frequency);

		// Trigger the envelope for the new note
		tailoffs[currentNoteForVoice].startNote();
		neverPlayedANoteBefore = 0;
	}

	// called to stop a note
	void stopNote(float velocity, bool allowTailOff) override {
		clearCurrentNote();

		// Stop the envelope for the note
		tailoffs[currentNoteForVoice].stopNote();

		// Iterate to the next slot in our array to prepare for the next note
		if (currentNoteForVoice + 1 >= waves.size()) {
			currentNoteForVoice = 0;
		}
		else{
			currentNoteForVoice += 1;
		}
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

		for (int i = 0; i < waves.size(); i++)
		{
			/*osc1.setType(paramMap.at("Oscillator1")->getValue(), numOscillators);
			osc1.adjustPitch(paramMap.at("Pitch1")->getValue(), freqs[i]);
			osc1.level = paramMap.at("Level1")->getValue();

			osc2.setType(paramMap.at("Oscillator2")->getValue(), numOscillators);
			osc2.adjustPitch(paramMap.at("Pitch2")->getValue(), freqs[i]);
			osc2.level = paramMap.at("Level2")->getValue();

			osc3.setType(paramMap.at("Oscillator3")->getValue(), numOscillators);
			osc3.adjustPitch(paramMap.at("Pitch3")->getValue(), freqs[i]);
			osc3.level = paramMap.at("Level3")->getValue();

			osc4.setType(paramMap.at("Oscillator4")->getValue(), numOscillators);
			osc4.adjustPitch(paramMap.at("Pitch4")->getValue(), freqs[i]);
			osc4.level = paramMap.at("Level4")->getValue();*/

			// Set the envelope parameters for each envelope
			tailoffs[i].setAttack(paramMap.at("Attack")->getValue() * 1000
				* paramScaleMap.at("Attack"));
			tailoffs[i].setDecay(paramMap.at("Decay")->getValue() * 1000
				* paramScaleMap.at("Decay"));
			tailoffs[i].setSustain(paramMap.at("Sustain")->getValue());
			tailoffs[i].setRelease(paramMap.at("Release")->getValue() * 1000
				* paramScaleMap.at("Release"));
		}

		//dist.setDistortion(paramMap.at("Distortion")->getValue() * paramScaleMap.at("Distortion"));

		//filter.setType(paramMap.at("Filter")->getValue(), numFilterTypes);
		//filter.setCutoffFreq(paramMap.at("Cutoff")->getValue() * paramScaleMap.at("Cutoff"));
		//filter.setResonanceBoost(paramMap.at("Resonance")->getValue() * paramScaleMap.at("Resonance"));

		//lfo.setType(paramMap.at("LFOosc")->getValue(), numOscillators);
		////lfo.(paramMap.at("LFOFreq")->getValue() * paramScaleMap.at("LFOFreq") + 1); // Min freq of 1Hz
		//lfo.level = paramMap.at("LFOLevel")->getValue();
		
	}

	//renders the next block of data for this voice
	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
		if (neverPlayedANoteBefore) return;		

		for (int sample = startSample; sample < numSamples; ++sample) {
			double outputWave = 0.0f;// = waves[currentNoteForVoice];
			
			//waves[currentNoteForVoice] = osc1.getWave() + osc2.getWave() + osc3.getWave() + osc4.getWave();

			// Here I'm trying to sum all the wave forms for every note on this oscillator voice.
			// I don't know why it doesn't work so well, it's making the pitch really high.
			// We have to apply each note its own tailoff envelope so that when the note is finished
			// no sound comes from it. So I'd think that most of the wave[i] values would not generate
			// sound because their envelopes trigger = 0, but I'm not so sure anymore.
			for (int i = 0; i < waves.size(); i++) {
				waves[i] = osc1.getWave() + osc2.getWave() + osc3.getWave() + osc4.getWave();
				waves[i] = tailoffs[i].apply(waves[i]);
				outputWave += waves[i];
			}
			
			// I think it works better when its just this one
			//waves[currentNoteForVoice] = osc1.getWave() + osc2.getWave() + osc3.getWave() + osc4.getWave();
			//waves[currentNoteForVoice] = tailoffs[currentNoteForVoice].apply(waves[currentNoteForVoice]);


			// Output the final wave product
			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
				outputBuffer.addSample(channel, sample, outputWave);
			}
		}
	}

private:
	int numOscillators = 4;
	int numFilterTypes = 4;
	double frequency;
	int neverPlayedANoteBefore = 1;
	
	Oscillator osc1;
	Oscillator osc2;
	Oscillator osc3;
	Oscillator osc4;
	Oscillator lfo;
	Distortion dist;
	Filter filter;
	
	array<double, 4> waves;
	int currentNoteForVoice = 0; // Allows us to know which array pos in waves we are are dealing with
	array<Envelope, 4> tailoffs;
	array<double, 4> freqs;
};
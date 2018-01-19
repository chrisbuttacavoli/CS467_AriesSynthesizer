/*
  ==============================================================================

    SynthAudioSource.h
    Created: 19 Jan 2018 10:40:23am
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "SineSynthVoice.h"

struct SynthAudioSource : public AudioSource
{
	SynthAudioSource(MidiKeyboardState &keyState) : keyboardState(keyState) {
		for (int i = 0; i < 5; i++)
		{
			mySynth.addVoice(new SineSynthVoice());
		}
		mySynth.clearSounds();
		mySynth.addSound(new SynthSound());
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		midiCollector.reset(sampleRate);

		mySynth.setCurrentPlaybackSampleRate(sampleRate);
	}

	void releaseResources() override
	{
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		// the synth always adds its output to the audio buffer, so we have to clear it
		// first..
		bufferToFill.clearActiveBufferRegion();

		// fill a midi buffer with incoming messages from the midi input.
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

		// pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.
		// This call will also add midi messages to the buffer which were generated by
		// the mouse-clicking on the on-screen keyboard.
		keyboardState.processNextMidiBuffer(incomingMidi, 0, bufferToFill.numSamples, true);

		// and now get the synth to process the midi events and generate its output.
		mySynth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
	}


	//Getting midi/keyboard messages
	MidiMessageCollector midiCollector;
	MidiKeyboardState& keyboardState;

	//the actual synth object
	Synthesiser mySynth;
};
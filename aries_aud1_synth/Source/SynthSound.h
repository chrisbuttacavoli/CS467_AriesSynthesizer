/*
  ==============================================================================

    SynthSound.h
    Created: 17 Jan 2018 9:56:51am
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class SynthSound : public SynthesiserSound {
	public:
		//return true if midi note is pressed
		bool appliesToNote(int midiNoteNumber) {
			return true;
		}
		//return true if sound should be triggered by midi events on given channel
		bool appliesToChannel(int midiChannel) {
			return true;
		}
	private:

};
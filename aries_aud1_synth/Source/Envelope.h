/*
  ==============================================================================

    Envelope.h
    Created: 2 Feb 2018 3:51:35pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once
#include "HelperFuncs.h"
#include "../JuceLibraryCode/JuceHeader.h"


class Envelope {
public:
	double apply(double wave) {
		//DBG(FloatToStr(wave));
		return env.adsr(wave, env.trigger);
	}

	void initializeForTesting() {
		env.setAttack(50);
		env.setDecay(1);
		env.setRelease(50);
		env.setSustain(50);
		env.amplitude = 0.1;
		env.trigger = 0;
	}

	void setAttack(double ms) {
		if (ms < minVal) {
			env.setAttack(minVal);
		}
		else {
			env.setAttack(ms);
		}
	}

	void setDecay(double ms) {
		if (ms < minVal) {
			env.setDecay(minVal);
		}
		else {
			env.setDecay(ms);
		}
	}

	void setSustain(double level) {
		env.setSustain(level);
	}

	void setRelease(double ms) {
		if (ms < minVal) {
			env.setRelease(minVal);
		}
		else {
			env.setRelease(ms);
		}
	}

	void startNote() {
		env.amplitude = 0.1;
		env.trigger = 1;
	}

	void stopNote() {
		env.trigger = 0;
	}

private:
	maxiEnv env;

	int minVal = 25;
};
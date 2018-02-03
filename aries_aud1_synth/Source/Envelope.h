/*
  ==============================================================================

    Envelope.h
    Created: 2 Feb 2018 3:51:35pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once

class Envelope {
public:
	double apply(double wave) {
		return env.adsr(wave, env.trigger);
	}

	void setAttack(double ms) {
		env.setAttack(ms + minAttack);
	}

	void setDecay(double ms) {
		env.setDecay(ms + minDecay);
	}

	void setSustain(double level) {
		env.setSustain(level);
	}

	void setRelease(double ms) {
		env.setRelease(ms + minRelease);
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

	int minAttack = 90;
	int minDecay = 90;
	int minRelease = 90;
};
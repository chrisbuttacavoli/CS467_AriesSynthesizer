/*
  ==============================================================================

    LFO.h
    Created: 6 Feb 2018 7:22:38pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class LFO {
public:
	LFO() : osc(OscillatorType::noWave) {
		
	}
	void setOscFreq(double freq) {
		osc.setFrequency(freq);
	}

	void setOscLevel(double level) {
		osc.level = level;
	}

	void setOscType(float theType, int numOscillators) {
		osc.setType(theType, numOscillators);
	}

	double apply(double wave) {
		if (osc.getType() == noWave || osc.level == 0) {
			return wave;
		}
		else {
			//frequency modulating LFO
			return (8.0f * (osc.getWave() * wave));
		}
	}
	

private:
	Oscillator osc;
};
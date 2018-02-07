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
		//return osc.getWave(freq);	//possibly overload oscillator class to take a freq
		return (osc.getWave() + wave);
	}

private:
	Oscillator osc;
	double freq;
};
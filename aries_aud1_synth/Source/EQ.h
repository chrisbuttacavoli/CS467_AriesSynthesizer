/*
  ==============================================================================

    EQ.h
    Created: 6 Feb 2018 7:22:22pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "Biquad.h"

class EQ {
public:
	double apply(double wave) {
		return theEQ.process(wave);
	}

	//create a peak filter
	void setFilter(double freq, double Q, double gain) {
		theEQ.setBiquad(4, freq, Q, gain);
	}

	/*void setFreq(double theFreq) {
		freq = theFreq;
	}

	void setQ(double theq) {
		q = theq;
	}

	void setLevel(double theGain) {
		level = theGain;
	}*/

private:
	Biquad theEQ;
	double freq;
	double q;
	double level;

};
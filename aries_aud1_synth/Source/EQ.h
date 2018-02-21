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
		theEQ.process(wave);
	}

	//create a peak filter
	void setFilter(double Fc, double Q, double peakGain) {
		theEQ.setBiquad(4, Fc, Q, peakGain);
	}

	void setFreq(double theFreq) {
		freq = theFreq;
	}

	void setQ(double theq) {
		q = theq;
	}

	void setLevel(double theGain) {
		level = theGain;
	}

private:
	Biquad theEQ;
	double freq;
	double q;
	double level;

};
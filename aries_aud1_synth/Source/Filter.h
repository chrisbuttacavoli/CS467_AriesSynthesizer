/*
  ==============================================================================

    Filter.h
    Created: 2 Feb 2018 6:50:48pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once
#include "Biquad.h"


enum FilterType {
	noFilter = 0,
	loPass = 1,
	hiPass = 2,
	bandPass = 3,
};

class Filter {
public:

	double apply(double wave) {
		switch (type)
		{
			case noFilter:
				return wave;

			case loPass:
				return filter.lores(wave, cutoff, resonance);

			case hiPass:
				return filter.hires(wave, cutoff, resonance);

			case bandPass:
				return biquad.process(wave);
		}
	}

	void setCutoffFreq(double freq) {
		cutoff = freq;
	}

	void setResonanceBoost(double boost) {
		resonance = boost;
	}

	// This is to pass info to the biquad
	void setSampleRate(int rate) {
		sampleRate = rate;
	}

	// Converts the float to an enum. paramVal ranges from 0.0 to 1.0
	void setType(float paramVal, int numFilterTypes) {
		type = static_cast<FilterType>(int(paramVal * (numFilterTypes - 1)));

		// Here we initialize the BiQuad parameters. Q is hardcoded in this class
		if (type == bandPass)
			biquad.setBiquad(bq_type_bandpass, cutoff / sampleRate, Q, resonance);
	}

private:
	int sampleRate = 44100;
	double cutoff;
	double resonance;
	maxiFilter filter;
	FilterType type;

	// BiQuad stuff
	Biquad biquad;
	double Q = 1.0f;
};
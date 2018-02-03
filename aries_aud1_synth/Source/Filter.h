/*
  ==============================================================================

    Filter.h
    Created: 2 Feb 2018 6:50:48pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once

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
		case loPass:
			return filter.lores(wave, cutoff, resonance);
			break;
		case hiPass:
			return filter.hires(wave, cutoff, resonance);
			break;
		case  bandPass:
			return filter.bandpass(wave, cutoff, resonance);
			break;
		}
		return wave;
	}

	void setCutoffFreq(double freq) {
		cutoff = freq;
	}

	// 1 to 200 seems good
	void setResonanceBoost(double boost) {
		resonance = boost;
	}

	// Converts the float to an enum. paramVal ranges from 0.0 to 1.0
	void setType(float paramVal, int numFilterTypes) {
		type = static_cast<FilterType>(int(paramVal * numFilterTypes));
	}

private:
	double cutoff;
	double resonance;
	maxiFilter filter;
	FilterType type;
};
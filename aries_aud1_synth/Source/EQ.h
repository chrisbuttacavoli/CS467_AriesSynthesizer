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
		if (Q <= minQ)
			Q = minQ;

		theEQ.setBiquad(bq_type_peak, freq, Q, gain);
	}

private:
	Biquad theEQ;
	double minQ = 0.05f;
};
/*
==============================================================================

Oscillator.h
Created: 27 Jan 2018 2:39:45pm
Author:  Chris.Buttacavoli

==============================================================================
*/

#pragma once

#include "maximilian.h"

enum OscillatorType
{
	none = 0,
	sineWave = 1,
	sawWave = 2,
	squareWave = 3,
	noiseWave = 4,
};

class Oscillator {
public:
	// Choose the oscillator type
	Oscillator(OscillatorType oscType) {
		type = oscType;
	}

	double level;

	// The frequency is adjusted by the GUI parameters. To change the
	// type of wave, change the type property.
	double getWave() {
		switch (type)
		{
		case sineWave:
			return osc.sinewave(freq) * level;
			break;
		case sawWave:
			return osc.saw(freq) * level;
			break;
		case squareWave:
			return osc.square(freq) * level;
			break;
		case noiseWave:
			return (random.nextFloat() * 0.25f - 0.125f) * level;
			break;
		default:
			break;
		}
		return 0;
	}

	// Octave val should between 0.0 and 1.0 from the GUI params
	void adjustPitch(float octaveVal, float referenceFreq) {
		// We only adjust +/- 1 octave
		freq = referenceFreq * pow(2, 2 * octaveVal - 1);
	}

	void initializePitch(float referenceFreq) {
		freq = referenceFreq;
		type = none;
	}

	void setType(OscillatorType oscType) {
		type = oscType;
	}

	// Converts the float to an enum. Ranges from 0.0 to 1.0
	void setType(float paramVal, int numOscillators) {
		float val = paramVal * 4;
		if (paramVal == 0) {
			//osc2.type = none;
			setType(none);
		}
		else if (val == 1) {
			//osc2.type = sineWave;
			setType(sineWave);
		}
		else if (val == 2) {
			//osc2.type = sawWave;
			setType(sawWave);
		}
		else if (val == 3) {
			//osc2.type = squareWave;
			setType(squareWave);
		}
		else if (val == 4) {
			//osc2.type = noiseWave;
			setType(noiseWave);
		}
	}

	//=======================================================
private:
	double freq;
	double pitch;
	maxiOsc osc;
	Random random; // For the noise oscillator
	OscillatorType type;
};
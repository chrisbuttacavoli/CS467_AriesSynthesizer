/*
  ==============================================================================

    Distortion.h
    Created: 3 Feb 2018 11:52:12am
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once


// Vic can use this class to write her own distortion algorithm eventually
class Distortion {
public:

	double apply(double wave) {
		return distortion.atanDist(wave, distortionAmount);
	}

	// Choose any positive value
	void setDistortion(double val) {
		distortionAmount = val + distortionMin;
	}

private:
	double distortionMin = 0.1f;
	double distortionAmount = 0.1f;

	maxiDistortion distortion;
};
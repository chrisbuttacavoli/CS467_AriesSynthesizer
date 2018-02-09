/*
  ==============================================================================

    HelperFuncs.h
    Created: 9 Feb 2018 12:38:55pm
    Author:  chris.buttacavoli

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// This is used for debugging: DBG(FloatToStr(someFloatValue))
#include <sstream>
String FloatToStr(float val) {
	std::ostringstream ss;
	ss << val;
	std::string s(ss.str());
	return s;
}
String DoubleToStr(double val) {
	std::ostringstream ss;
	ss << val;
	std::string s(ss.str());
	return s;
}
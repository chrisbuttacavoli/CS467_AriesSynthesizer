/*
  ==============================================================================

    FileReader.h
    Created: 2 Mar 2018 12:06:53pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "HelperFuncs.h"


class PatchManager {
public:

	Array<float> GetParamsFromFile(File file) {
		StringArray strArr;
		file.readLines(strArr);

		Array<String> lines = strArr.strings;
		Array<float> params;
		for (int i = 0; i < lines.size(); i++)
		{
			String line = lines[i];
			if (line.isNotEmpty()) {
				params.add(line.getFloatValue());
			}
		}
		return params;
	}


	void SaveParamsToFile(const OwnedArray<AudioProcessorParameter>& params, File file) {
		file.deleteFile();
		file.create();

		for (int i = 0; i < params.size(); i++)
		{
			float paramValue = params[i]->getValue();
			file.appendText(FloatToStr(paramValue));
			file.appendText("\n");
		}
	}
};
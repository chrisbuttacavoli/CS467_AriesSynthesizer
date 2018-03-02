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


class FileReader {
public:

	//std::vector<double> GetParamsFromFile() {
	Array<float> GetParamsFromFile(juce::String fileName) {
		//String line = getFileLine(fileName);
		juce::StringArray strArr;
		const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(fileName));
		file.readLines(strArr);

		Array<String> lines = strArr.strings;
		Array<float> params;
		for (int i = 0; i < lines.size(); i++)
		{
			String line = lines[i];
			params.add(line.getFloatValue());
		}

		return params;
	}
};
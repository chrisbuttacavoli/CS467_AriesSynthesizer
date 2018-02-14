/*
==============================================================================

This file is part of the JUCE library.
Copyright (c) 2017 - ROLI Ltd.

JUCE is an open source library subject to commercial or open-source
licensing.

By using JUCE, you agree to the terms of both the JUCE 5 End-User License
Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
27th April 2017).

End User License Agreement: www.juce.com/juce-5-licence
Privacy Policy: www.juce.com/juce-5-privacy-policy

Or: You may also use this code under the terms of the GPL v3 (see
www.gnu.org/licenses).

JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
DISCLAIMED.

==============================================================================
*/

//FROM JUCE DEMO

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/** A simple class that acts as an AudioIODeviceCallback and writes the
incoming audio data to a WAV file.
*/
class AudioRecorder : public AudioIODeviceCallback
{
public:
	AudioRecorder(AudioThumbnail& thumbnailToUpdate)
		: thumbnail(thumbnailToUpdate),
		backgroundThread("Audio Recorder Thread"),
		sampleRate(0), nextSampleNum(0), activeWriter(nullptr)
	{
		backgroundThread.startThread();
	}

	~AudioRecorder()
	{
		stop();
	}

	//==============================================================================
	void startRecording(const File& file)
	{
		stop();

		if (sampleRate > 0)
		{
			// Create an OutputStream to write to our destination file...
			file.deleteFile();
			ScopedPointer<FileOutputStream> fileStream(file.createOutputStream());

			if (fileStream != nullptr)
			{
				// Now create a WAV writer object that writes to our output stream...
				WavAudioFormat wavFormat;
				AudioFormatWriter* writer = wavFormat.createWriterFor(fileStream, sampleRate, 1, 16, StringPairArray(), 0);

				if (writer != nullptr)
				{
					fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

										  // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
										  // write the data to disk on our background thread.
					threadedWriter = new AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768);

					// Reset our recording thumbnail
					thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
					nextSampleNum = 0;

					// And now, swap over our active writer pointer so that the audio callback will start using it..
					const ScopedLock sl(writerLock);
					activeWriter = threadedWriter;
				}
			}
		}
	}

	void stop()
	{
		// First, clear this pointer to stop the audio callback from using our writer object..
		{
			const ScopedLock sl(writerLock);
			activeWriter = nullptr;
		}

		// Now we can delete the writer object. It's done in this order because the deletion could
		// take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
		// the audio callback while this happens.
		threadedWriter = nullptr;
	}

	bool isRecording() const
	{
		return activeWriter != nullptr;
	}

	//==============================================================================
	void audioDeviceAboutToStart(AudioIODevice* device) override
	{
		sampleRate = device->getCurrentSampleRate();
	}

	void audioDeviceStopped() override
	{
		sampleRate = 0;
	}

	//records outputbuffer data to a file in mono!
	//TODO: Try to make this record in stereo? - Victoria
	void audioDeviceIOCallback(const float** inputChannelData, int /*numInputChannels*/,
		float** outputChannelData, int numOutputChannels,
		int numSamples) override
	{
		const ScopedLock sl(writerLock);

		if (activeWriter != nullptr)
		{
			activeWriter->write(outputChannelData, numSamples);

			// Create an AudioSampleBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
			const AudioSampleBuffer buffer(const_cast<float**> (outputChannelData), 2, numSamples);
			thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
			nextSampleNum += numSamples;
		}
	}

private:
	AudioThumbnail& thumbnail;
	TimeSliceThread backgroundThread; // the thread that will write our audio data to disk
	ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
	double sampleRate;
	int64 nextSampleNum;

	CriticalSection writerLock;
	AudioFormatWriter::ThreadedWriter* volatile activeWriter;
};

/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.h"
#include "SynthSound.h"
#include "SynthProcessor.h"
#include "GenericEditor.h"
#include "AudioRecorder.h"
#include "RecordingThumbnail.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public AudioAppComponent,
	private ComboBox::Listener,
	private Button::Listener,
	private MidiInputCallback,
	private MidiKeyboardStateListener
{
public:
    //==============================================================================
    MainContentComponent()
		: deviceManager(AudioAppComponent::deviceManager),
		lastInputIndex(0),
		isAddingFromMidiInput(false),
		keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
		synthProcessor(keyboardState),
		recorder(recordingThumbnail.getAudioThumbnail()),	//initializing the recorder object
		startTime(Time::getMillisecondCounterHiRes() * 0.001)

    {
		setOpaque(true);

		mySynth.clearVoices();

		theEditor = static_cast <GenericEditor*>(synthProcessor.createEditor());

		addAndMakeVisible(theEditor);
		theEditor->setSize(800, 400);

		//Debug prints display box
		addAndMakeVisible(midiInputListLabel);
		midiInputListLabel.setText("MIDI Input:", dontSendNotification);
		midiInputListLabel.attachToComponent(&midiInputList, true);

		//MIDI input list display
		addAndMakeVisible(midiInputList);
		midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
		const StringArray midiInputs(MidiInput::getDevices());
		midiInputList.addItemList(midiInputs, 1);
		midiInputList.addListener(this);

		// find the first enabled device and use that by default
		for (int i = 0; i < midiInputs.size(); ++i)
		{
			if (deviceManager.isMidiInputEnabled(midiInputs[i]))
			{
				setMidiInput(i);
				break;
			}
		}

		// if no enabled devices were found just use the first one in the list
		if (midiInputList.getSelectedId() == 0)
			setMidiInput(0);

        // specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);

		//making keyboard visible
		addAndMakeVisible(keyboardComponent);
		keyboardState.addListener(this);

		//setting up an audio player to actually output audio
		audioSourcePlayer.setProcessor(&synthProcessor); //for synthProcessor class

		//device manager to deal with midi/devices
		deviceManager.addAudioCallback(&audioSourcePlayer);
		deviceManager.addMidiInputCallback(String(), (&synthProcessor.midiCollector));

		//recording button
		// IMAGE BUTTON CODE FOR RECORD BUTTON

		addAndMakeVisible(recordButton);
		Image juceImage = ImageCache::getFromMemory(BinaryData::record_s_s_png, BinaryData::record_s_s_pngSize);
		recordButton->setImages(true, true, true,
			juceImage, 0.7f, Colours::transparentBlack,
			juceImage, 1.0f, Colours::transparentBlack,
			juceImage, 1.0f, Colours::pink.withAlpha(0.8f),
			0.5f);
		recordButton->setTopLeftPosition(666, 400);
		recordButton->setTooltip("image button - showing alpha-channel hit-testing and colour overlay when clicked");
		// END IMAGE BUTTON CODE

		// IMAGE BUTTON CODE FOR SAVE BUTTON

		addAndMakeVisible(saveButton);
		Image juceImage2 = ImageCache::getFromMemory(BinaryData::save_s_png, BinaryData::save_s_pngSize);
		saveButton->setImages(true, true, true,
			juceImage2, 0.7f, Colours::transparentBlack,
			juceImage2, 1.0f, Colours::transparentBlack,
			juceImage2, 1.0f, Colours::pink.withAlpha(0.8f),
			0.5f);
		saveButton->setTopLeftPosition(670, 490);
		saveButton->setTooltip("image button - showing alpha-channel hit-testing and colour overlay when clicked");
		// END IMAGE BUTTON CODE
		// IMAGE BUTTON CODE FOR SAVE BUTTON

		addAndMakeVisible(loadButton);
		Image juceImage3 = ImageCache::getFromMemory(BinaryData::load_s_png, BinaryData::load_s_pngSize);
		loadButton->setImages(true, true, true,
			juceImage3, 0.7f, Colours::transparentBlack,
			juceImage3, 1.0f, Colours::transparentBlack,
			juceImage3, 1.0f, Colours::pink.withAlpha(0.8f),
			0.5f);
		loadButton->setTopLeftPosition(670, 450);
		loadButton->setTooltip("image button - showing alpha-channel hit-testing and colour overlay when clicked");
		// END IMAGE BUTTON CODE

		//addAndMakeVisible(recordButton);
		//imageButton.setButtonText("Record");
		recordButton->addListener(this);
		loadButton->addListener(this);
		//imageButton.addListener(this);
		//addAndMakeVisible(recordingThumbnail);	//displays what is being recorded. Maybe not necessary?
		deviceManager.addAudioCallback(&recorder);

		//text input
		Colour(255,0,255);
		addAndMakeVisible(inputLabel);
		inputLabel.setText("Filename:", dontSendNotification);
		inputLabel.setFont(Font(16.0f, Font::bold));
		inputLabel.attachToComponent(&inputText, true);
		inputLabel.setColour(Label::textColourId, Colour(255, 0, 255));
		inputLabel.setJustificationType(Justification::right);
		addAndMakeVisible(inputText);
		inputText.setEditable(true);
		inputText.setColour(Label::backgroundColourId, Colours::white);
		inputText.setColour(Label::textColourId, Colours::black);
		inputText.setColour(Label::textWhenEditingColourId, Colours::black);
		inputText.setColour(Label::outlineWhenEditingColourId, Colour(255, 0, 255));

		//setting the size of the windows
		setSize(1060, 690);

    }

	//destructor
    ~MainContentComponent()
    {
        shutdownAudio();
		keyboardState.removeListener(this);
		deviceManager.removeMidiInputCallback(MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
		midiInputList.removeListener(this);
		deviceManager.removeAudioCallback(&recorder);
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		//ignore unused samples to remove garbage from last key press
		ignoreUnused(samplesPerBlockExpected);
		lastSampleRate = sampleRate;
		mySynth.setCurrentPlaybackSampleRate(lastSampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        //Currently using SynthProcessor.h to play audio
    }

    void releaseResources() override
    {
        //releasing resources in SynthProcessor.h
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

        // "Logo" stuff
		Font theFont("Helvetica", "Bold", 25.0f);
		g.setFont(theFont);
		//g.drawText("Aries Synthesizer", -10, 400, getWidth(), 50, Justification::right);
		Image logo = ImageCache::getFromMemory(BinaryData::aries_logo_cropped_s_png, BinaryData::aries_logo_cropped_s_pngSize);
		g.drawImageAt(logo, 500, 500);
		Font theFont2("Helvetica", "Bold", 10.0f);
		g.setFont(theFont2);
		g.drawText("Chris B, Victoria D, Alex C", 10, 600, getWidth(), 25, Justification::bottomLeft);
    }

	//placing objects inside the main window
    void resized() override
    {
		juce::Rectangle<int> area(getLocalBounds());

		//displaying midi input list, the processor editor, and the keyboard
		midiInputList.setBounds(area.removeFromTop(36).removeFromRight(getWidth() - 100).reduced(8));
		theEditor->setBounds(area.removeFromTop(595));
		keyboardComponent.setBounds(0, 545, getWidth(), 150);
		//recordingThumbnail.setBounds(area.removeFromTop(80).reduced(8));	//displays what is being recorded. Maybe not necessary?
		//recordButton.setBounds(10, 350, 75, 25);

		inputText.setBounds(835, 515, (area.getWidth() / 4.8), 25);
	}

private:
	//==============================================================================
	// Your private member variables go here...
	AudioDeviceManager& deviceManager;           // [1]
	ComboBox midiInputList;                     // [2]
	Label midiInputListLabel;
	int lastInputIndex;                         // [3]
	bool isAddingFromMidiInput;                 // [4]
	MidiKeyboardState keyboardState;            // [5]
	MidiKeyboardComponent keyboardComponent;    // [6]
	double startTime;
	ImageButton* recordButton = new ImageButton("imagebutton");
	ImageButton* saveButton = new ImageButton("savebutton");
	ImageButton* loadButton = new ImageButton("loadbutton");
	AudioProcessorPlayer audioSourcePlayer;	//need to play audio through a processor player now
	SynthProcessor synthProcessor;		//new synth processor to replay synthAudioSource
	GenericEditor *theEditor;

	Synthesiser mySynth;
	double lastSampleRate;

	//Recording Objects
	RecordingThumbnail recordingThumbnail;
	AudioRecorder recorder;
	//TextButton recordButton;
	Label inputLabel;
	Label inputText;

	/** Starts listening to a MIDI input device, enabling it if necessary. */
	void setMidiInput(int index)
	{
		const StringArray list(MidiInput::getDevices());

		deviceManager.removeMidiInputCallback(list[lastInputIndex], this);

		const String newInput(list[index]);

		if (!deviceManager.isMidiInputEnabled(newInput))
			deviceManager.setMidiInputEnabled(newInput, true);

		deviceManager.addMidiInputCallback(newInput, this);
		midiInputList.setSelectedId(index + 1, dontSendNotification);

		lastInputIndex = index;
	}

	void comboBoxChanged(ComboBox* box) override
	{
		if (box == &midiInputList)
			setMidiInput(midiInputList.getSelectedItemIndex());
	}

	// These methods handle callbacks from the midi device + on-screen keyboard..
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
		keyboardState.processNextMidiEvent(message);
	}

	void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
	{
		if (!isAddingFromMidiInput)
		{
			MidiMessage m(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
			m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
		}

		mySynth.noteOn(midiChannel, midiNoteNumber, velocity);
	}

	void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override
	{
		if (!isAddingFromMidiInput)
		{
			MidiMessage m(MidiMessage::noteOff(midiChannel, midiNoteNumber));
			m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
		}
	}

	//Recording function/Button clicked function
	void startRecording()
	{
		//naming file and starting recording
		//TODO: Create a way for the user to enter in their file name and saving location

		const File file(File::getSpecialLocation(File::userDocumentsDirectory)
			.getNonexistentChildFile(inputText.getText(), ".wav"));
		recorder.startRecording(file);
		Image juceImage4 = ImageCache::getFromMemory(BinaryData::stop_s_png, BinaryData::stop_s_pngSize);
		recordButton->setImages(true, true, true,
			juceImage4, 0.7f, Colours::transparentBlack,
			juceImage4, 1.0f, Colours::transparentBlack,
			juceImage4, 1.0f, Colours::pink.withAlpha(0.8f),
			0.5f);
		recordButton->setTopLeftPosition(666, 400);
	}

	void stopRecording()
	{
		recorder.stop();
		Image juceImage = ImageCache::getFromMemory(BinaryData::record_s_s_png, BinaryData::record_s_s_pngSize);
		recordButton->setImages(true, true, true,
			juceImage, 0.7f, Colours::transparentBlack,
			juceImage, 1.0f, Colours::transparentBlack,
			juceImage, 1.0f, Colours::pink.withAlpha(0.8f),
			0.5f);
		recordButton->setTopLeftPosition(666, 400);
		recordButton->setTooltip("image button - showing alpha-channel hit-testing and colour overlay when clicked");
	}

	void buttonClicked(Button* button) override
	{
		if (button == recordButton)
		{
			if (recorder.isRecording())
				stopRecording();
			else
				startRecording();
		}
		else if (button == loadButton) {
			synthProcessor.loadPatch();
		}
	}

	//==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }

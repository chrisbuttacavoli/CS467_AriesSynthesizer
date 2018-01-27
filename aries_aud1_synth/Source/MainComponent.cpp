/*
==============================================================================

This file was auto-generated!

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Enums.h"
#include "Oscillator.h"
#include "OscillatorVoice.h"
#include "SynthSound.h"
#include "SynthProcessor.h"
#include "GenericEditor.h"


//==============================================================================
/*
This component lives inside our window, and this is where you should put all
your controls and content.
*/
class MainContentComponent : public AudioAppComponent,
	private ComboBox::Listener,
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
		theSynthProcessor(keyboardState),
		//synthAudioSource (keyboardState),
		startTime(Time::getMillisecondCounterHiRes() * 0.001)
	{
		setOpaque(true);

		mySynth.clearVoices();

		theEditor = static_cast <GenericEditor*>(theSynthProcessor.createEditor());
		//addChildComponent(theEditor);
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

		addOscillatorControls();

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
		setAudioChannels(0, 2);


		//making keyboard visible
		addAndMakeVisible(keyboardComponent);
		keyboardState.addListener(this);

		//actually displaying the midi messages
		/*addAndMakeVisible(midiMessagesBox);
		midiMessagesBox.setMultiLine(true);
		midiMessagesBox.setReturnKeyStartsNewLine(true);
		midiMessagesBox.setReadOnly(true);
		midiMessagesBox.setScrollbarsShown(true);
		midiMessagesBox.setCaretVisible(false);
		midiMessagesBox.setPopupMenuEnabled(true);
		midiMessagesBox.setColour(TextEditor::backgroundColourId, Colour(0x32ffffff));
		midiMessagesBox.setColour(TextEditor::outlineColourId, Colour(0x1c000000));
		midiMessagesBox.setColour(TextEditor::shadowColourId, Colour(0x16000000));*/

		//setting up an audio player to actually output audio
		audioSourcePlayer.setProcessor(&theSynthProcessor); //for synthProcessor class

															//device manager to deal with midi/devices
		deviceManager.addAudioCallback(&audioSourcePlayer);
		deviceManager.addMidiInputCallback(String(), (&theSynthProcessor.midiCollector));

		//setting the size of the windows
		setSize(900, 600);

	}

	//destructor
	~MainContentComponent()
	{
		shutdownAudio();
		keyboardState.removeListener(this);
		deviceManager.removeMidiInputCallback(MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
		midiInputList.removeListener(this);
		//cbOsc1.removeListener(this);
	}

	//==============================================================================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		// This function will be called when the audio device is started, or when
		// its settings (i.e. sample rate, block size, etc) are changed.

		// You can use this function to initialise any resources you might need,
		// but be careful - it will be called on the audio thread, not the GUI thread.

		// For more details, see the help for AudioProcessor::prepareToPlay()

		//ignore unused samples to remove garbage from last key press
		ignoreUnused(samplesPerBlockExpected);
		lastSampleRate = sampleRate;
		mySynth.setCurrentPlaybackSampleRate(lastSampleRate);
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		//Currently using SynthAudioSource.h to play audio
	}

	void releaseResources() override
	{
		//releasing resources in SynthAudioSource.h
	}

	//==============================================================================
	void paint(Graphics& g) override
	{
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		//theEditor->paint(g);
		// You can add your drawing code here!
		Font theFont("Helvetica", "Bold", 50.0f);
		g.setFont(theFont);
		g.drawText("Aries Synthesizer", 25, 25, 400, 60, Justification::topRight);
		Font theFont2("Helvetica", "Bold", 20.0f);
		g.setFont(theFont2);
		g.drawText("Chris B, Victoria D, Alex C", 50, 30, 400, 60, Justification::bottomLeft);
	}

	//placing objects inside the main window
	void resized() override
	{
		// This is called when the MainContentComponent is resized.
		// If you add any child components, this is where you should
		// update their positions.
		juce::Rectangle<int> area(getLocalBounds());
		/*midiInputList.setBounds(area.removeFromTop(36).removeFromRight(getWidth() - 150).reduced(8));
		keyboardComponent.setBounds(area.removeFromBottom(80).reduced(8));
		midiMessagesBox.setBounds(area.reduced(8));*/
		osc1.setBounds(50, 120, (area.getWidth() / 4.8), 125);
		osc2.setBounds(250, 120, (area.getWidth() / 4.8), 125);
		osc3.setBounds(450, 120, (area.getWidth() / 4.8), 125);
		osc4.setBounds(650, 120, (area.getWidth() / 4.8), 125);
		//theEditor->setBounds(0, 40, (area.getWidth() / 4), 100);
		midiInputList.setBounds(area.removeFromTop(36).removeFromRight(getWidth() - 100).reduced(8));
		//theEditor->setBounds(area.removeFromTop(50).removeFromRight(getWidth() - 100).reduced(8)); // This didn't give enough height to the component
		theEditor->setBounds(0, 250, getWidth(), 250); // hardcoded positions
		keyboardComponent.setBounds(0, 450, getWidth(), 150);
	}

private:
	//==============================================================================
	//MIDI debug prints from: https://juce.com/doc/tutorial_handling_midi_events
	static String getMidiMessageDescription(const MidiMessage& m)
	{
		if (m.isNoteOn())           return "Note on " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
		if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);

		if (m.isController())
		{
			String name(MidiMessage::getControllerName(m.getControllerNumber()));

			if (name.isEmpty())
				name = "[" + String(m.getControllerNumber()) + "]";

			return "Controller " + name + ": " + String(m.getControllerValue());
		}

		return String::toHexString(m.getRawData(), m.getRawDataSize());
	}

	void logMessage(const String& m)
	{
		midiMessagesBox.moveCaretToEnd();
		midiMessagesBox.insertTextAtCaret(m + newLine);
	}

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

		/*if (box == &cbOsc1)
		DBG(cbOsc1.getSelectedIdAsValue().toString());*/
	}

	// These methods handle callbacks from the midi device + on-screen keyboard..
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
		keyboardState.processNextMidiEvent(message);
		postMessageToList(message, source->getName());
	}

	void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
	{
		if (!isAddingFromMidiInput)
		{
			MidiMessage m(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
			m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
			postMessageToList(m, "On-Screen Keyboard");
		}

		mySynth.noteOn(midiChannel, midiNoteNumber, velocity);
	}

	void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override
	{
		if (!isAddingFromMidiInput)
		{
			MidiMessage m(MidiMessage::noteOff(midiChannel, midiNoteNumber));
			m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
			postMessageToList(m, "On-Screen Keyboard");
		}
	}

	// This is used to dispach an incoming message to the message thread
	class IncomingMessageCallback : public CallbackMessage
	{
	public:
		IncomingMessageCallback(MainContentComponent* o, const MidiMessage& m, const String& s)
			: owner(o), message(m), source(s)
		{}

		void messageCallback() override
		{
			if (owner != nullptr)
				owner->addMessageToList(message, source);
		}

		Component::SafePointer<MainContentComponent> owner;
		MidiMessage message;
		String source;
	};

	void postMessageToList(const MidiMessage& message, const String& source)
	{
		(new IncomingMessageCallback(this, message, source))->post();
	}

	void addMessageToList(const MidiMessage& message, const String& source)
	{
		const double time = message.getTimeStamp() - startTime;

		const int hours = ((int)(time / 3600.0)) % 24;
		const int minutes = ((int)(time / 60.0)) % 60;
		const int seconds = ((int)time) % 60;
		const int millis = ((int)(time * 1000.0)) % 1000;

		const String timecode(String::formatted("%02d:%02d:%02d.%03d",
			hours,
			minutes,
			seconds,
			millis));

		const String description(getMidiMessageDescription(message));

		const String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
		logMessage(midiMessageString);
	}


	void addOscillatorControls() {
		/*addAndMakeVisible(lblOsc1);
		lblOsc1.setText("Oscillator 1:", dontSendNotification);
		lblOsc1.attachToComponent(&cbOsc1, true);
		addAndMakeVisible(cbOsc1);
		cbOsc1.addItem("Sine", sine);
		cbOsc1.addItem("Saw", saw);
		cbOsc1.addListener(this);
		cbOsc1.setSelectedId(sine);*/
		addAndMakeVisible(osc1);
		addAndMakeVisible(osc2);
		addAndMakeVisible(osc3);
		addAndMakeVisible(osc4);
	}

	//==============================================================================

	// Your private member variables go here...
	AudioDeviceManager& deviceManager;           // [1]
	ComboBox midiInputList;                     // [2]
	Label midiInputListLabel;
	int lastInputIndex;                         // [3]
	bool isAddingFromMidiInput;                 // [4]
	MidiKeyboardState keyboardState;            // [5]
	MidiKeyboardComponent keyboardComponent;    // [6]
	TextEditor midiMessagesBox;
	double startTime;

	//AudioSourcePlayer audioSourcePlayer;
	//SynthAudioSource synthAudioSource;
	AudioProcessorPlayer audioSourcePlayer;	//need to play audio through a processor player now
	SynthProcessor theSynthProcessor;		//new synth processor to replay synthAudioSource
	GenericEditor *theEditor;

	Synthesiser mySynth;
	double lastSampleRate;

	// Controls
	/*ComboBox cbOsc1;
	Label lblOsc1;*/
	Oscillator osc1;
	Oscillator osc2;
	Oscillator osc3;
	Oscillator osc4;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }
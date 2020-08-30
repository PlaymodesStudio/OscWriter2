/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define FLOATPARAMETERSIZE 8
#define FLAGPARAMETERSIZE 2

//==============================================================================
OscWriter2AudioProcessor::OscWriter2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for(int i = 0; i < FLOATPARAMETERSIZE; i++){
        floatParameters.push_back(new AudioParameterFloat("Float Parameter" + String(i), "Parameter " + String(i), NormalisableRange<float>(0.0000f, 1.0000f), 0.0));
        addParameter(floatParameters.back());
        
        oscAddresses.add("/param" + String(i));
    }
    for(int i = 0; i < FLAGPARAMETERSIZE; i++){
        boolParameters.push_back(new AudioParameterBool("Bool Parameter" + String(i), "Bool Parameter " + String(i), false));
        addParameter(boolParameters.back());
        
        oscAddresses.add("/flag" + String(i));
    }
    oscHost = "127.0.0.1";
    oscPort = "12345";
    presetName = "Initial_Bank/1--Init";
    newPresetLoadFlag = false;
}

OscWriter2AudioProcessor::~OscWriter2AudioProcessor()
{
}

//==============================================================================
const String OscWriter2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OscWriter2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OscWriter2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OscWriter2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OscWriter2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OscWriter2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OscWriter2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void OscWriter2AudioProcessor::setCurrentProgram (int index)
{
}

const String OscWriter2AudioProcessor::getProgramName (int index)
{
    return {};
}

void OscWriter2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OscWriter2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void OscWriter2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OscWriter2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OscWriter2AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    
    playHead = this->getPlayHead();
    playHead->getCurrentPosition (currentPositionInfo);
}

//==============================================================================
bool OscWriter2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OscWriter2AudioProcessor::createEditor()
{
    return new OscWriter2AudioProcessorEditor (*this);
}

//==============================================================================
void OscWriter2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    MemoryOutputStream stream (destData, true);
    
    for (int i = 0; i < oscAddresses.size(); i++){
        stream.writeString (oscAddresses[i]);
    }
    stream.writeString("127.0.0.1"); //This is not used, but is keep for compatibility
    stream.writeString("12345"); //This is not used, but is keep for compatibility
    stream.writeString(presetName);
}

void OscWriter2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);
    
    for (int i = 0; i < oscAddresses.size(); i++){
        oscAddresses.set(i, stream.readString());
    }
    String sillyOscHost = stream.readString(); //This is not used, but is keep for compatibility
    String sillyOscPort = stream.readString(); //This is not used, but is keep for compatibility
    presetName = stream.readString();
    newPresetLoadFlag = true;
}

void OscWriter2AudioProcessor::setOscAddress(String address, int index)
{
    oscAddresses.set(index, address);
}

StringArray OscWriter2AudioProcessor::getOscAddress(){
    return oscAddresses;
}

String OscWriter2AudioProcessor::getOscAddressByIndex(int index){
    return oscAddresses[index];
}

bool OscWriter2AudioProcessor::getFlag()
{
    return newPresetLoadFlag;
}

void OscWriter2AudioProcessor::setFlagFalse(){
    newPresetLoadFlag = false;
}

float OscWriter2AudioProcessor::getBpm(){
    if(playHead != nullptr){
        return currentPositionInfo.bpm;
    }
    return 0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OscWriter2AudioProcessor();
}

/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class OscWriter2AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    OscWriter2AudioProcessor();
    ~OscWriter2AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void setOscAddress(String address, int index);
    StringArray getOscAddress();
    String getOscAddressByIndex(int index);
    bool getFlag();
    void setFlagFalse();
    
    String getOscHost(){return oscHost;};
    void setOscHost(String host){oscHost = host;};
    
    String getOscPort(){return oscPort;};
    void setOscPort(String port){oscPort = port;};

private:
    std::vector<AudioParameterFloat*> floatParameters;
    std::vector<AudioParameterBool*>  boolParameters;
    StringArray  oscAddresses;
    String oscHost;
    String oscPort;
    
    bool newPresetLoadFlag;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscWriter2AudioProcessor)
};

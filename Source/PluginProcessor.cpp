/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQThroughWallsAudioProcessor::EQThroughWallsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
}

EQThroughWallsAudioProcessor::~EQThroughWallsAudioProcessor()
{
}

//==============================================================================
const juce::String EQThroughWallsAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool EQThroughWallsAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool EQThroughWallsAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool EQThroughWallsAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double EQThroughWallsAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int EQThroughWallsAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int EQThroughWallsAudioProcessor::getCurrentProgram()
{
	return 0;
}

void EQThroughWallsAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String EQThroughWallsAudioProcessor::getProgramName(int index)
{
	return {};
}

void EQThroughWallsAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void EQThroughWallsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;
	spec.sampleRate = sampleRate;

	leftChain.prepare(spec);
	rightChain.prepare(spec);

	auto chainSettings = getChainSettings(apvts);

	auto wallCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		convertWallsToFrequency(chainSettings.highCutFreq),
		sampleRate,
		2 * (convertWallsToFrequency(chainSettings.highCutFreq) + 2));

	auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();

	leftHighCut.setBypassed<0>(true);
	leftHighCut.setBypassed<1>(true);
	leftHighCut.setBypassed<2>(true);
	leftHighCut.setBypassed<3>(true);

	auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

	rightHighCut.setBypassed<0>(true);
	rightHighCut.setBypassed<1>(true);
	rightHighCut.setBypassed<2>(true);
	rightHighCut.setBypassed<3>(true);

	if (chainSettings.highCutFreq != zero)
	{
		switch (chainSettings.highCutSlope)
		{
		case Slope_12:
		{

			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			break;
		}
		case Slope_24:
		{
			if (chainSettings.highCutFreq == zero)
			{
				break;
			}
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			break;
		}
		case Slope_32:
		{
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			*leftHighCut.get<2>().coefficients = *wallCoefficients[2];
			leftHighCut.setBypassed<2>(false);
			break;
		}
		case Slope_48:
		{
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			*leftHighCut.get<2>().coefficients = *wallCoefficients[2];
			leftHighCut.setBypassed<2>(false);
			*leftHighCut.get<3>().coefficients = *wallCoefficients[3];
			leftHighCut.setBypassed<3>(false);
			break;
		}
		}


		switch (chainSettings.highCutSlope)
		{
		case Slope_12:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			break;
		}
		case Slope_24:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			break;
		}
		case Slope_32:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			*rightHighCut.get<2>().coefficients = *wallCoefficients[2];
			rightHighCut.setBypassed<2>(false);
			break;
		}
		case Slope_48:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			*rightHighCut.get<2>().coefficients = *wallCoefficients[2];
			rightHighCut.setBypassed<2>(false);
			*rightHighCut.get<3>().coefficients = *wallCoefficients[3];
			rightHighCut.setBypassed<3>(false);
			break;
		}
		}
	}
}

void EQThroughWallsAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQThroughWallsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void EQThroughWallsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep 
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	auto chainSettings = getChainSettings(apvts);

	auto wallCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		convertWallsToFrequency(chainSettings.highCutFreq),
		getSampleRate(),
		2 * (convertWallsToFrequency(chainSettings.highCutFreq) + 2));


	auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();

	leftHighCut.setBypassed<0>(true);
	leftHighCut.setBypassed<1>(true);
	leftHighCut.setBypassed<2>(true);
	leftHighCut.setBypassed<3>(true);

	auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

	rightHighCut.setBypassed<0>(true);
	rightHighCut.setBypassed<1>(true);
	rightHighCut.setBypassed<2>(true);
	rightHighCut.setBypassed<3>(true);

	if (chainSettings.highCutFreq != zero)
	{
		switch (chainSettings.highCutSlope)
		{
		case Slope_12:
		{

			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			break;
		}
		case Slope_24:
		{
			if (chainSettings.highCutFreq == zero)
			{
				break;
			}
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			break;
		}
		case Slope_32:
		{
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			*leftHighCut.get<2>().coefficients = *wallCoefficients[2];
			leftHighCut.setBypassed<2>(false);
			break;
		}
		case Slope_48:
		{
			*leftHighCut.get<0>().coefficients = *wallCoefficients[0];
			leftHighCut.setBypassed<0>(false);
			*leftHighCut.get<1>().coefficients = *wallCoefficients[1];
			leftHighCut.setBypassed<1>(false);
			*leftHighCut.get<2>().coefficients = *wallCoefficients[2];
			leftHighCut.setBypassed<2>(false);
			*leftHighCut.get<3>().coefficients = *wallCoefficients[3];
			leftHighCut.setBypassed<3>(false);
			break;
		}
		}

		switch (chainSettings.highCutSlope)
		{
		case Slope_12:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			break;
		}
		case Slope_24:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			break;
		}
		case Slope_32:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			*rightHighCut.get<2>().coefficients = *wallCoefficients[2];
			rightHighCut.setBypassed<2>(false);
			break;
		}
		case Slope_48:
		{
			*rightHighCut.get<0>().coefficients = *wallCoefficients[0];
			rightHighCut.setBypassed<0>(false);
			*rightHighCut.get<1>().coefficients = *wallCoefficients[1];
			rightHighCut.setBypassed<1>(false);
			*rightHighCut.get<2>().coefficients = *wallCoefficients[2];
			rightHighCut.setBypassed<2>(false);
			*rightHighCut.get<3>().coefficients = *wallCoefficients[3];
			rightHighCut.setBypassed<3>(false);
			break;
		}
		}
	}

	juce::dsp::AudioBlock<float> block(buffer);

	auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);

	juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
	juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

	leftChain.process(leftContext);
	rightChain.process(rightContext);
}

//==============================================================================
bool EQThroughWallsAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQThroughWallsAudioProcessor::createEditor()
{
	//return new EQThroughWallsAudioProcessorEditor (*this);
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void EQThroughWallsAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void EQThroughWallsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
	ChainSettings settings;

	settings.highCutFreq = static_cast<WallValues> (apvts.getRawParameterValue("Walls In Between")->load());
	settings.highCutSlope = static_cast<Slope> (apvts.getRawParameterValue("Material")->load());

	return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout
EQThroughWallsAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	juce::StringArray stringArray = { "Glass", "Wood", "Drywall", "Concrete" };

	juce::StringArray wallsArray;
	for (int i = 0; i < 5; i++)
	{
		juce::String str;
		str << i;
		wallsArray.add(str);
	}

	layout.add(std::make_unique <juce::AudioParameterChoice>("Material", "Material", stringArray, 0));

	layout.add(std::make_unique <juce::AudioParameterChoice>("Walls In Between", "Walls In Between", wallsArray, 0));

	return layout;
};

float EQThroughWallsAudioProcessor::convertWallsToFrequency(WallValues wallCount)
{
	switch (wallCount)
	{
	case zero:
	{
		return 7500.f;
	}
	case one:
	{
		return 3000.f;
	}
	case two:
	{
		return 1500.f;
	}
	case three:
	{
		return 750.f;
	}
	case four:
	{
		return 100.f;
	}
	}
	return 0.f;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new EQThroughWallsAudioProcessor();
}

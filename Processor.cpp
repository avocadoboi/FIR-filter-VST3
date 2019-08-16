#include "Processor.hpp"
#include "Controller.hpp"
#include "pluginterfaces\vst\ivstparameterchanges.h"
#include "pluginterfaces\base\ibstream.h"
#include "global.hpp"
#include "base\source\fstreamer.h"

//---------------------------------------------

Processor::Processor() : m_filter(NUMBER_OF_INPUT_WEIGHTS, NUMBER_OF_OUTPUT_WEIGHTS) {
	setControllerClass(Controller::classID);
}

//---------------------------------------------

tresult PLUGIN_API Processor::initialize(FUnknown* p_context) {
	tresult result = AudioEffect::initialize(p_context);

	if (result != kResultOk) {
		return result;
	}

	addAudioInput(STR16("Stereo in"), SpeakerArr::kStereo);
	addAudioOutput(STR16("Stereo out"), SpeakerArr::kStereo);
	addEventInput(STR16("Event in"), 1);

	return kResultOk;
}
tresult PLUGIN_API Processor::process(ProcessData& p_data) {
	// Get new parameter value
	for (int i_parameter = 0; i_parameter < p_data.inputParameterChanges->getParameterCount(); i_parameter++) {
		IParamValueQueue* parameterValueQueue = p_data.inputParameterChanges->getParameterData(i_parameter);

		int ID = parameterValueQueue->getParameterId();
		long sampleOffset;
		double parameterValue;
		if (parameterValueQueue->getPoint(parameterValueQueue->getPointCount() - 1, sampleOffset, parameterValue) == kResultTrue) {
			//if (ID < NUMBER_OF_INPUT_WEIGHTS) {
				m_filter.setInputHistoryWeight(ID, parameterValue*2. - 1.);
			//}
			//else {
			//	m_filter.setOutputHistoryWeight(ID - NUMBER_OF_INPUT_WEIGHTS, parameterValue*2. - 1.);
			//}
		}
	}

	if (processSetup.symbolicSampleSize == kSample32) {
		processAudio<float>(p_data.inputs->channelBuffers32, p_data.outputs->channelBuffers32, p_data.inputs->numChannels, p_data.numSamples);
	}
	else if (processSetup.symbolicSampleSize == kSample64) {
		processAudio<double>(p_data.inputs->channelBuffers64, p_data.outputs->channelBuffers64, p_data.inputs->numChannels, p_data.numSamples);
	}

	return kResultOk;
}

tresult PLUGIN_API Processor::getState(IBStream* p_data) {
	for (int a = 0; a < NUMBER_OF_INPUT_WEIGHTS; a++) {
		double value = m_filter.getInputHistoryWeight(a);
		p_data->write(&value, sizeof(double));
	}
	//for (int a = 0; a < NUMBER_OF_OUTPUT_WEIGHTS; a++) {
	//	double value = m_filter.getOutputHistoryWeight(a);
	//	p_data->write(&value, sizeof(double));
	//}

	return kResultOk;
}
tresult PLUGIN_API Processor::setState(IBStream* p_data) {
	IBStreamer streamer(p_data);
	for (uint32_t a = 0; a < NUMBER_OF_INPUT_WEIGHTS; a++) {
		double value;
		streamer.readDouble(value);
		m_filter.setInputHistoryWeight(a, value);
	}
	//for (uint32_t a = 0; a < NUMBER_OF_OUTPUT_WEIGHTS; a++) {
	//	double value;
	//	streamer.readDouble(value);
	//	m_filter.setOutputHistoryWeight(a, value);
	//}

	return kResultOk;
}

//---------------------------------------------

FUnknown* Processor::createInstance(void*) {
	return (IAudioProcessor*)new Processor();
}

FUID Processor::classID(3751155872, 83969591, 2683806524, 1895397592);

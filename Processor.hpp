#pragma once
#include "public.sdk\source\vst\vstaudioeffect.h"
#include "Filter.hpp"

using namespace Steinberg;
using namespace Steinberg::Vst;

//---------------------------------------------

class Processor : public AudioEffect {
private:
	ExperimentalFilter m_filter;

public:
	Processor();

	//------------------------------------------------

	template<class SampleType>
	void processAudio(SampleType** p_in, SampleType** p_out, int p_numberOfChannels, int p_numberOfSamples) {
		for (int i_channel = 0; i_channel < p_numberOfChannels; i_channel++) {
			for (int i_sample = 0; i_sample < p_numberOfSamples; i_sample++) {
				p_out[i_channel][i_sample] = SampleType(m_filter.getProcessed(p_in[i_channel][i_sample], i_channel));
			}
		}
	}

	//------------------------------------------------
	// From AudioEffect

	tresult PLUGIN_API initialize(FUnknown* p_context);
	tresult PLUGIN_API process(ProcessData& p_data);

	tresult PLUGIN_API getState(IBStream* p_data);
	tresult PLUGIN_API setState(IBStream* p_data);

	//---------------------------------------------

	static FUnknown* createInstance(void*);
	static FUID classID;
};
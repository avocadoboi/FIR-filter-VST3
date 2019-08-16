#pragma once
#include "base\source\fdebug.h"

enum class FilterType {
	Lowpass,
	Highpass
};

class ExperimentalFilter {
private:
	double* m_inputHistoryWeights;
	double** m_inputHistory;
	int m_inputHistoryLength;

	double* m_outputHistoryWeights;
	double** m_outputHistory;
	int m_outputHistoryLength;

	double m_weightSum;

public:
	ExperimentalFilter(int p_inputHistoryLength, int p_outputHistoryLength);
	~ExperimentalFilter();

	//---------------------------------------------

	void setInputHistoryWeight(int p_index, double p_weight);
	double getInputHistoryWeight(int p_index);

	void setOutputHistoryWeight(int p_index, double p_weight);
	double getOutputHistoryWeight(int p_index);

	//---------------------------------------------

	double getProcessed(double p_input, int p_channel);
};

class WindowedSincFilter {
private:
	FilterType m_type;

	double** m_inputHistory;
	double* m_kernel;
	int m_kernelLength = 25 * 2 + 1;

	double m_frequency = 0.4; // Fraction of sample rate

	//---------------------------------------------

	void generateKernel();
	void generateKernelWithNewSize();

public:
	WindowedSincFilter(FilterType p_type = FilterType::Lowpass);
	~WindowedSincFilter();

	//---------------------------------------------

	void setKernelLength(int p_kernelLength);
	int getKernelLength();

	void setFrequency(double p_frequency);
	double getFrequency();

	void setType(FilterType p_type);
	FilterType getType();

	//---------------------------------------------

	double getProcessed(double p_input, int p_channel);
};

class SimpleFilter {
private:
	FilterType m_type;

	double** m_filterChannels;
	double* m_lastInput; // Pointer because two channels

	double m_frequency = 0;
	double m_resonance = 0.9;
	int m_steepness;

public:
	SimpleFilter(FilterType p_type = FilterType::Lowpass, int p_steepness = 8);
	~SimpleFilter();

	//---------------------------------------------

	void setFrequency(double p_frequency);
	double getFrequency();

	void setResonance(double p_resonance);
	double getResonance();

	//---------------------------------------------

	double getProcessed(double p_input, int p_channel);
};

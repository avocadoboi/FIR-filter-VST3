#include "Filter.hpp"
#include "global.hpp"
#include <math.h>

//---------------------------------------------
//
//		   class ExperimentalFilter
//
//---------------------------------------------

ExperimentalFilter::ExperimentalFilter(int p_inputHistoryLength, int p_outputHistoryLength) :
	m_inputHistoryLength(p_inputHistoryLength),
	m_outputHistoryLength(p_outputHistoryLength), m_weightSum(0)
{
	if (m_inputHistoryLength > 0) {
		m_inputHistory = getNewChannelPointerToArray(m_inputHistoryLength);
		m_inputHistoryWeights = new double[m_inputHistoryLength];
		for (int a = 0; a < m_inputHistoryLength; a++) {
			m_inputHistoryWeights[a] = 0;
		}
	}

	if (m_outputHistoryLength > 0) {
		m_outputHistory = getNewChannelPointerToArray(m_outputHistoryLength);
		m_outputHistoryWeights = new double[m_outputHistoryLength];
		for (int a = 0; a < m_outputHistoryLength; a++) {
			m_outputHistoryWeights[a] = 0;
		}
	}
}
ExperimentalFilter::~ExperimentalFilter() {
	if (m_inputHistoryLength > 0) {
		delete[] m_inputHistory[0];
		delete[] m_inputHistory[1];
		delete[] m_inputHistory;

		delete[] m_inputHistoryWeights;
	}

	//---------------------------------------------

	if (m_outputHistoryLength > 0) {
		delete[] m_outputHistory[0];
		delete[] m_outputHistory[1];
		delete[] m_outputHistory;

		delete[] m_outputHistoryWeights;
	}
}

//---------------------------------------------

void ExperimentalFilter::setInputHistoryWeight(int p_index, double p_weight) {
	m_weightSum += (fabs(p_weight) - fabs(m_inputHistoryWeights[p_index]));
	m_inputHistoryWeights[p_index] = p_weight;
}
double ExperimentalFilter::getInputHistoryWeight(int p_index) {
	return m_inputHistoryWeights[p_index];
}

void ExperimentalFilter::setOutputHistoryWeight(int p_index, double p_weight) {
	m_weightSum += (fabs(p_weight) - fabs(m_outputHistoryWeights[p_index]));
	m_outputHistoryWeights[p_index] = p_weight;
}
double ExperimentalFilter::getOutputHistoryWeight(int p_index) {
	return m_outputHistoryWeights[p_index];
}

//---------------------------------------------

double ExperimentalFilter::getProcessed(double p_input, int p_channel) {
	// Spring lowpass :^)
	//m_velocity[p_channel] *= m_resonance/(1+m_frequency);
	//m_velocity[p_channel] += m_frequency * (p_input - m_position[p_channel]);
	//m_position[p_channel] += m_velocity[p_channel];
	//return m_position[p_channel];

	double output = 0;

	double* valuePointer;
	double* weightPointer;
	double* end;

	if (m_inputHistoryLength > 0) {
		valuePointer = m_inputHistory[p_channel];
		weightPointer = m_inputHistoryWeights;
		end = valuePointer + m_inputHistoryLength - 1;
		if (m_weightSum == 0.) {
			while (valuePointer != end) {
				*valuePointer = *(valuePointer + 1);
				valuePointer++;
			}
			*valuePointer = p_input;
		}
		else {
			while (valuePointer != end) {
				*valuePointer = *(valuePointer + 1);
				output += *(valuePointer++) * *(weightPointer++);
			}
			*valuePointer = p_input;
			output += *valuePointer * *weightPointer;
		}
	}

	if (m_outputHistoryLength > 0) {
		valuePointer = m_outputHistory[p_channel];
		weightPointer = m_outputHistoryWeights;
		end = valuePointer + m_outputHistoryLength - 1;
		if (m_weightSum == 0.) {
			while (valuePointer != end) {
				*valuePointer = *(valuePointer + 1);
				valuePointer++;
			}
			*valuePointer = output;
		}
		else {
			while (valuePointer != end) {
				*valuePointer = *(valuePointer + 1);
				output += *(valuePointer++) * *(weightPointer++);
			}
			output += *valuePointer * *weightPointer;
			*valuePointer = output;
		}
	}
	output /= m_weightSum;

	return output;
}

//---------------------------------------------
//
//			class ConvolutionFilter
//
//---------------------------------------------

//
// Private
//

void WindowedSincFilter::generateKernel() {
	double sum = 0;
	for (int a = 0; a < m_kernelLength; a++) {
		if (a == (m_kernelLength - 1)*0.5) {
			m_kernel[a] = 2 * m_frequency;
		}
		else {
			double shiftedIndex = a - (double)(m_kernelLength - 1) * 0.5;
			m_kernel[a] = sin(2.0 * PI*m_frequency*shiftedIndex) / (PI*shiftedIndex) * (0.54 - 0.46 * cos((double)a * 2.0 * PI / (m_kernelLength - 1)));
		}
		sum += m_kernel[a];
	}
	if (sum > 0) {
		for (int a = 0; a < m_kernelLength; a++) {
			m_kernel[a] /= sum;
		}
	}
}
void WindowedSincFilter::generateKernelWithNewSize() {
	for (int i_channel = 0; i_channel < 2; i_channel++) {
		if (m_inputHistory[i_channel] == nullptr) {
			delete[] m_inputHistory[i_channel];
		}
		m_inputHistory[i_channel] = new double[m_kernelLength];

		for (int i_sample = 0; i_sample < m_kernelLength; i_sample++) {
			m_inputHistory[i_channel][i_sample] = 0;
		}
	}

	//---------------------------------------------

	if (m_kernel != nullptr) {
		delete[] m_kernel;
	}
	m_kernel = new double[m_kernelLength];

	generateKernel();
}

//
// Public
//

WindowedSincFilter::WindowedSincFilter(FilterType p_type) {
	m_type = p_type;
	m_inputHistory = new double*[2];
	generateKernelWithNewSize();
}
WindowedSincFilter::~WindowedSincFilter() {
	delete[] m_inputHistory[0];
	delete[] m_inputHistory[1];
	delete[] m_inputHistory;

	//---------------------------------------------

	delete[] m_kernel;
}

//---------------------------------------------

void WindowedSincFilter::setKernelLength(int p_kernelLength) {
	m_kernelLength = p_kernelLength * 2 + 1;
	generateKernelWithNewSize();
}
int WindowedSincFilter::getKernelLength() {
	return m_kernelLength;
}

void WindowedSincFilter::setFrequency(double p_frequency) {
	m_frequency = getMin(0.5, getMax(0.0, p_frequency*0.5));
	generateKernel();
}
double WindowedSincFilter::getFrequency() {
	return m_frequency;
}

void WindowedSincFilter::setType(FilterType p_type) {
	if (p_type != m_type) {
		m_type = p_type;
		generateKernel();
	}
}
FilterType WindowedSincFilter::getType() {
	return m_type;
}

//---------------------------------------------

double WindowedSincFilter::getProcessed(double p_input, int p_channel) {
	double output = 0;
	for (int i_sample = 0; i_sample < m_kernelLength - 1; i_sample++) {
		m_inputHistory[p_channel][i_sample] = m_inputHistory[p_channel][i_sample + 1];
		output += m_inputHistory[p_channel][i_sample] * m_kernel[i_sample];
	}
	m_inputHistory[p_channel][m_kernelLength - 1] = p_input;
	output += p_input * m_kernel[m_kernelLength - 1];

	if (m_type == FilterType::Lowpass) {
		return output;
	}
	else {
		return m_inputHistory[p_channel][int((m_kernelLength - 1) * 0.5)] - output;
	}
}

//---------------------------------------------
//
//			  class SimpleFilter
//
//---------------------------------------------

SimpleFilter::SimpleFilter(FilterType p_type, int p_steepness) {
	m_type = p_type;
	m_steepness = p_steepness;

	m_filterChannels = new double*[2];
	for (int i_channel = 0; i_channel < 2; i_channel++) {
		m_filterChannels[i_channel] = new double[p_steepness];
		for (int i_sample = 0; i_sample < p_steepness; i_sample++) {
			m_filterChannels[i_channel][i_sample] = 0;
		}
	}

	if (m_type == FilterType::Highpass) {
		m_lastInput = new double[2];
		m_lastInput[0] = 0;
		m_lastInput[1] = 0;
	}
}
SimpleFilter::~SimpleFilter() {
	delete[] m_filterChannels[0];
	delete[] m_filterChannels[1];
	delete[] m_filterChannels;

	if (m_type == FilterType::Highpass) {
		delete[] m_lastInput;
	}
}

//---------------------------------------------

void SimpleFilter::setFrequency(double p_frequency) {
	m_frequency = p_frequency;
}
double SimpleFilter::getFrequency() {
	return m_frequency;
}

void SimpleFilter::setResonance(double p_resonance) {
	m_resonance = p_resonance;
}
double SimpleFilter::getResonance() {
	return m_resonance;
}

//---------------------------------------------

double SimpleFilter::getProcessed(double p_input, int p_channel) {
	double* f = m_filterChannels[p_channel];

	if (m_type == FilterType::Lowpass) {
		for (int i_filterSample = 0; i_filterSample < m_steepness; i_filterSample++) {
			if (i_filterSample < m_steepness - 1) {
				if (i_filterSample > 0) {
					f[i_filterSample] += m_frequency * (f[i_filterSample - 1] - f[i_filterSample]) + m_frequency * m_resonance * (f[i_filterSample] - f[i_filterSample + 1]);
				}
				else {
					f[i_filterSample] += m_frequency * (p_input - f[i_filterSample]) + m_frequency * m_resonance * (f[i_filterSample] - f[i_filterSample + 1]);
				}
			}
			else {
				if (i_filterSample > 0) {
					f[i_filterSample] += m_frequency * (f[i_filterSample - 1] - f[i_filterSample]);
				}
				else {
					f[i_filterSample] += m_frequency * (p_input - f[i_filterSample]);
				}
			}
		}
	}
	else if (m_type == FilterType::Highpass) {
		double lastLayerInput = 0;
		for (int i_filterSample = 0; i_filterSample < m_steepness; i_filterSample++) {
			double before = f[i_filterSample];
			if (i_filterSample < m_steepness - 1) {
				if (i_filterSample > 0) {
					f[i_filterSample] = (1 - m_frequency) * (f[i_filterSample] + f[i_filterSample - 1] - lastLayerInput) + m_frequency * m_resonance * (f[i_filterSample + 1] - f[i_filterSample]);
				}
				else {
					f[i_filterSample] = (1 - m_frequency) * (f[i_filterSample] + p_input - m_lastInput[p_channel]) + m_frequency * m_resonance * (f[i_filterSample + 1] - f[i_filterSample]);
				}
			}
			else {
				if (i_filterSample > 0) {
					f[i_filterSample] = (1 - m_frequency) * (f[i_filterSample] + f[i_filterSample - 1] - lastLayerInput);
				}
				else {
					f[i_filterSample] = (1 - m_frequency) * (f[i_filterSample] + p_input - m_lastInput[p_channel]);
				}
			}
			lastLayerInput = before;
		}
		m_lastInput[p_channel] = p_input;
	}

	return f[m_steepness - 1];
}

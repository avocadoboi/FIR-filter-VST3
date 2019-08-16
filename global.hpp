#pragma once

//---------------------------------------------
// Macros

#define PI 3.14159265359
#define NUMBER_OF_INPUT_WEIGHTS 51
#define NUMBER_OF_OUTPUT_WEIGHTS 0//101

//---------------------------------------------
// Functions

template<class T>
inline T getClipped(T p_number, T p_min, T p_max) {
	return p_number < p_min ? p_min : (p_number > p_max ? p_max : p_number);
}

template<class T>
inline T getMin(T p_a, T p_b) {
	if (p_a < p_b) {
		return p_a;
	}
	else {
		return p_b;
	}
}

template<class T>
inline T getMax(T p_a, T p_b) {
	if (p_a > p_b) {
		return p_a;
	}
	else {
		return p_b;
	}
}

template<class T = double, class T_Data = double>
extern T* getNewChannelPointer(T_Data p_initialData = 0) {
	T* channelPointer = new T[2];
	channelPointer[0] = p_initialData;
	channelPointer[1] = p_initialData;
	return channelPointer;
}

template<class T = double>
extern T** getNewChannelPointerToArray(int p_arrayLength) {
	T** channels = new T*[2];
	channels[0] = new T[p_arrayLength];
	channels[1] = new T[p_arrayLength];
	for (int a = 0; a < p_arrayLength; a++) {
		channels[0][a] = 0;
		channels[1][a] = 0;
	}
	return channels;
}

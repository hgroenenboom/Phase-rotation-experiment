/*
  ==============================================================================

    SmoothIIRFloat.h
    Created: 11 Mar 2019 11:17:55am
    Author:  HAROL

  ==============================================================================
*/

#pragma once

class IIRFloat {
public:
	IIRFloat(int smoothTime = 50) 
		: smoothTime(smoothTime), invSmoothTime(1.0f / smoothTime)
	{

	}

	void operator =(float a) {
		valueToReach = a;
		tick();
	}

	void operator +=(float a) {
		valueToReach += a;
		tick();
	}

	void tick() {
		filter(valueToReach);
	}

	// Constant operators
	const bool operator < (float a) {
		return value < a;
	}

	const float operator() (){
		return value;
	}

	const bool operator== (float a) {
		return value == a;
	}

	// Getters
	const float getUnfiltered() {
		return value;
	}

	// Setters
	void setUnfiltered(float value) {
		this->value = value;
	}

	void setSmoothTime(int steps) {
		steps = steps < 1 ? 1 : steps;
		smoothTime = steps;
		invSmoothTime = 1.0f / steps;
	}
private:
	void filter(float a) {
		value = (1.0f - invSmoothTime) * value + invSmoothTime * a;
	}

	int smoothTime = 1;
	float invSmoothTime = 1.0f;
	float value = 0.0f;
	float valueToReach = 0.0f;
};
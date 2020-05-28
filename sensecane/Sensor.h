#pragma once

template<typename T>
class Sensor
{
public:
	Sensor(int _id) { id = _id;};
	virtual void setup() = 0;
	virtual T get() = 0;
	int id;
};

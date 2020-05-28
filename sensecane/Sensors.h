#pragma once

#include "Sensor.h"
#include <Wire.h>
#include <HX711_ADC.h>
#include <PulseSensorPlayground.h>
#include <Adafruit_MLX90614.h>

/********************************************************************************/
/********************************************************************************/
class SensorTemperatura : public Sensor<double>
{
public:
  SensorTemperatura(int id) : Sensor<double>(id) { this->sensor = new Adafruit_MLX90614(MLX90614_I2CADDR); }
  void setup();
	double get();
private:
  Adafruit_MLX90614* sensor;
};

inline void SensorTemperatura::setup()
{
  this->sensor->begin();
}

inline double SensorTemperatura::get() {
	return this->sensor->readObjectTempC();
}

/********************************************************************************/
/********************************************************************************/
class SensorPresion: public Sensor<float>
{
public:
  SensorPresion(int id, uint8_t dout, uint8_t sck) : Sensor<float>(id) { sensor = new HX711_ADC(dout, sck); }
  void setup();
	float get();
private:
  HX711_ADC* sensor;
};

inline void SensorPresion::setup()
{
	this->sensor->start(2000);
  this->sensor->setCalFactor(-41.95);
  this->sensor->begin();
}

inline float SensorPresion::get() {
	return this->sensor->getData();
}

/********************************************************************************/
/********************************************************************************/
class SensorPulso: public Sensor<int>
{
public:
  SensorPulso(int id, int _threshold, int _pulsewire): Sensor<int>(id) { sensor = new PulseSensorPlayground(1); threshold = _threshold; pulsewire = _pulsewire; }
  void setup();
	int get();
private:
  PulseSensorPlayground* sensor;
  int threshold;
  int pulsewire;
};

inline void SensorPulso::setup()
{
	this->sensor->analogInput(this->pulsewire);
  this->sensor->setThreshold(this->threshold);
  this->sensor->begin();
}

inline int SensorPulso::get() {
	return this->sensor->sawStartOfBeat() ? this->sensor->getBeatsPerMinute() : -1;
}

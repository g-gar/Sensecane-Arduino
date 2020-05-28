#include "Sensor.h"
#include <TimerThree.h>

class Executor
{
public:
	virtual void start();
	virtual void stop();
  virtual void execute();
};

class SensecaneExecutor : public Executor
{
public:
  SensecaneExecutor(int _idUser);
private:
  int id_user;
  SensorPulso* pulso;
  SensorPresion* presion;
  SensorTemperatura* temperatura;
};

inline SensecaneExecutor::SensecaneExecutor(int id_user)
{
  this->id_user = id_user;
  this->pulso = new SensorPulso(1, threshold, pulsewire);
  this->presion = new SensorPresion(2, dout, sck);
  this->temperatura = new SensorTemperatura(3);
}

inline void start()
{
  
}

inline void stop()
{
  
}

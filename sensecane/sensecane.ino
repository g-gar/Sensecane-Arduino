/********************************************************************************
    Mas informacion Comados AT: https://www.itead.cc/wiki/ESP8266_Serial_WIFI_Module

    Cableado:
    Arduino Mega       ||  Adaptador ESP8266
      TX1-18           ||  RX
      RX-19            ||  TX
      5V               ||  VCC
      GND              ||  GND
********************************************************************************/

#define USE_ARDUINO_INTERRUPTS true

#include <ArduinoJson.h>
#include <TimerThree.h>
#include <QueueArray.h>
#include "sensors.h"
#include "AT.h"
#include <TimedAction.h>

volatile QueueArray<String> queue;

using namespace AT;

void readCardiacFrequency();
void readTemperature();
void readPressure();

const uint8_t user_id = ; // add
const uint8_t dout = 4;
const uint8_t sck = 5;
const uint8_t threshold = 550;
const uint8_t pulsewire = 0;

const char* ssid = ""; // add
const char* password = ""; // add

const char* dbUser = ""; // add
const char* dbPass = ""; // add
const char* dbName = ""; // add


SensorPulso* pulso;
SensorPresion* presion;
SensorTemperatura* temperatura;

int pulseSensorInterruptTimer = 2;
int temperatureSensorInterruptTimer = 3;
int pressureSensorInterruptTimer = 16;

long t;

TimedAction pulseThread = TimedAction(pulseSensorInterruptTimer, readCardiacFrequency);
TimedAction temperatureThread = TimedAction(temperatureSensorInterruptTimer, readTemperature);
TimedAction pressureThread = TimedAction(pressureSensorInterruptTimer * 1, readPressure);

/********************************************************************************/
/********************************************************************************/
void setup() {
    Serial.begin( 115200 ); // Connection to PC
    Serial1.begin( 115200 ); // Connection to ESP8266

    sendComand( "AT" );
    if( !WaitOK( 1000 ) ) {
        Serial.println( "Error: Modulo Wifi no responde" );
        return;
    }

    // Esto es para dar tiempo a que se conecte a la wireless..
    delay( 5000 );

    sendComand( "AT+CIPSTART=\"TCP\",\""+String(ip)+"\","+String(port)+"" );
    WaitOK( 2000 );
    sendComand( "AT+CIPSTATUS" );
    WaitOK( 1000 );

    pulso = new SensorPulso(1, threshold, pulsewire);
    presion = new SensorPresion(2, dout, sck);
    temperatura = new SensorTemperatura(3);

    pulso->setup();
    presion->setup();
    temperatura->setup();
}

/********************************************************************************/
/********************************************************************************/
void loop() {    
    //respuesta = WaitAnswer( 1000 );

    pulseThread.check();
    //temperatureThread.check();
    //pressureThread.check();
    
    //if (millis() > t + pulseSensorInterruptTimer * 1000) {
       //readCardiacFrequency();
       //t = millis();
    //}

    String item;
    if (!queue.isEmpty()) {
      item = utf8ascii(queue.dequeue());
      Serial.println(item);
      sendPost( "/sensors/data/", item);
    }
}

/********************************************************************************/
/********************************************************************************/
void readCardiacFrequency()
{
  int freq = pulso->get();
  String body = "{\"user_id\":\""+ String(user_id) +"\",\"sensor_id\":\""+ String(pulso->id) +"\",\"value\":\""+ freq +"\"}";

  //DynamicJsonDocument doc(1024);
  //doc["user_id"] = user_id;
  //doc["sensor_id"] = 1;
  //doc["value"] = freq;

  //serializeJson(doc, body);
  
  queue.enqueue(body);
}

/********************************************************************************/
/********************************************************************************/
void readTemperature()
{
  double temp = temperatura->get();
  String body = "{\"user_id\": "+ String(user_id) +", \"sensor_id\": "+ String(temperatura->id) +", \"value\": "+ temp +"}";

  //DynamicJsonDocument doc(1024);
  //doc["user_id"] = user_id;
  //doc["sensor_id"] = 2;
  //doc["value"] = temp;

  //serializeJson(doc, body);
  
  //Serial.println(body);
  queue.enqueue(body);
}

/********************************************************************************/
/********************************************************************************/
void readPressure()
{
  float pres = presion->get();
  String body = "{\"user_id\": "+ String(user_id) +", \"sensor_id\": "+ String(presion->id) +", \"value\": "+ pres +"}";
  //Serial.println(body);
  queue.enqueue(body);
}

// ****** UTF8-Decoder: convert UTF8-string to extended ASCII *******
static byte c1;  // Last character buffer

// Convert a single Character from UTF8 to Extended ASCII
// Return "0" if a byte has to be ignored
byte utf8ascii(byte ascii) {
    if ( ascii<128 )   // Standard ASCII-set 0..0x7F handling  
    {   c1=0;
        return( ascii );
    }

    // get previous input
    byte last = c1;   // get last char
    c1=ascii;         // remember actual character

    switch (last)     // conversion depending on first UTF8-character
    {   case 0xC2: return  (ascii);  break;
        case 0xC3: return  (ascii | 0xC0);  break;
        case 0x82: if(ascii==0xAC) return(0x80);       // special case Euro-symbol
    }

    return  (0);                                     // otherwise: return zero, if character has to be ignored
}

// convert String object from UTF8 String to Extended ASCII
String utf8ascii(String s)
{      
        String r="";
        char c;
        for (int i=0; i<s.length(); i++)
        {
                c = utf8ascii(s.charAt(i));
                if (c!=0) r+=c;
        }
        return r;
}

// In Place conversion UTF8-string to Extended ASCII (ASCII is shorter!)
void utf8ascii(char* s)
{      
        int k=0;
        char c;
        for (int i=0; i<strlen(s); i++)
        {
                c = utf8ascii(s[i]);
                if (c!=0)
                        s[k++]=c;
        }
        s[k]=0;
}

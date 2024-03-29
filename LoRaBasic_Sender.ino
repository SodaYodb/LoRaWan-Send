#include "LoRaWan_APP.h"
#include "Arduino.h"

#include <DHT.h>
#define DHT1_PIN 31
DHT dht_top(DHT1_PIN, DHTTYPE);
float hum1, temp1;

static const uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // msb
static const uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // msb
static const uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //msb
//uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

TimerEvent_t sleepTimer;
bool sleepTimerExpired;

static void wakeUp()
{
  sleepTimerExpired=true:}

static void lowPowerSleep(uint32_t sleeptime)
{
  sleepTimerExpired=false;
  TimerInit( &sleepTimer, &wakeUp );
  TimerSetValue( &sleepTimer, sleeptime );
  TimerStart( &sleepTimer );
  //Low power handler also gets interrupted by other timers
  while (!sleepTimerExpired) lowPowerHandler();
  TimerStop( &sleepTimer );}

void setup()
{
  Serial.begin(115200);
  LoRaWAN.init(CLASS_A, LORAMAC_REGION_EU868);}

void loop(){
  uint32_t starttime = millis();
  hum1 = dht_top.readHumidity();
  temp1 = dht_top.readTemperature();
  Send_Data(hum1, temp1);
  lowPowerSleep(60000);}

void Send_Data(float hum, float temp) {
  // convert humidity and temperature to integers without decimal point
  // keep in mind that the temperature or humidity could be greater than 99.9
  int hum_int = (int)(hum * 10);
  int temp_int = (int)(temp * 10);
  // create a buffer to hold the data as a string
  // be aware of this 20 byte buffer size, maybe check the lenght of data you want provide or make buffer dynamic or just do both
  char data[20];
  int data_length = sprintf(data, "%d,%d", temp_int, hum_int);
  
  LoRaWAN.join(devEui, appEui, appKey);
  if (LoRaWAN.joined()) {
    LoRaWAN.send(appPort, (uint8_t*)data, data_length, isTxConfirmed);
  } else {
    Serial.println("FAILED TO JOIN LORA");
  }
}

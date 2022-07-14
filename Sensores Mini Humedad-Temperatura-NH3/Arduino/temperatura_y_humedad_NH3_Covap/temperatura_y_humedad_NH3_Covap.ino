
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define RL 47  //The value of resistor RL is 47K
#define m -0.263 //Enter calculated Slope 
#define b 0.42 //Enter calculated intercept
#define Ro 20 //Enter found Ro value

/*
   set LoraWan_RGB to Active,the RGB active in loraWan
   RGB red means sending;
   RGB purple means joined done;
   RGB blue means RxWindow1;
   RGB yellow means RxWindow2;
   RGB green means received done;
*/

/*
 function Decoder(bytes, port) {
  var Temperatura = (bytes[0] << 8) | bytes[1];
  var Humedad = (bytes[2] << 8) | bytes[3];
  var Bateria = (bytes[4] << 8) | bytes[5];
  
    
  var decoded = {};
  
  if(Temperatura / 100 > 100)
  {
    decoded.Temperatura = 0;
  }else{
    decoded.Temperatura = Temperatura / 100;
  }
  
   if(Humedad / 100 > 100)
  {
    decoded.Humedad = 0;
  }else{
    decoded.Humedad = Humedad / 100;
  }
  

  decoded.Bateria = Bateria;

  return decoded;
}
 */


/* OTAA para*/
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xCF, 0xF8 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x7D, 0xCC, 0xC8, 0xEE, 0x69, 0x9C, 0x34, 0x21, 0x7E, 0xA4, 0xB9, 0x21, 0xCC, 0x5E, 0x8A, 0xE1 };

/* ABP para*/
uint8_t nwkSKey[] = {0x38, 0x4B, 0x23, 0xF5, 0x9D, 0x0F, 0xFE, 0x27, 0x50, 0xDD, 0x3A, 0xBF, 0x19, 0xBE, 0x82, 0x14};
uint8_t appSKey[] = {0x4C, 0xC0, 0x89, 0xFA, 0x67, 0xB8, 0x5F, 0x2A, 0xC0, 0xAF, 0xD7, 0x17, 0x1B, 0x37, 0xC4, 0xC8};
uint32_t devAddr =  ( uint32_t )0x260B3E97;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 900000; // 1800000; 30m

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
  Number of trials to transmit the frame, if the LoRaMAC layer did not
  receive an acknowledgment. The MAC performs a datarate adaptation,
  according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
  to the following table:

  Transmission nb | Data Rate
  ----------------|-----------
  1 (first)       | DR
  2               | DR
  3               | max(DR-1,0)
  4               | max(DR-1,0)
  5               | max(DR-2,0)
  6               | max(DR-2,0)
  7               | max(DR-3,0)
  8               | max(DR-3,0)

  Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
  the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

/* Prepares the payload of the frame */


static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
    appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
    if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
    if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
    for example, if use REGION_CN470,
    the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
   delay(50);

  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        digitalWrite(Vext, HIGH);
        delay(1000);
    }



  float temperature = sht31.readTemperature();
    float humidity = sht31.readHumidity();
    Wire.end();
  
  digitalWrite(Vext, HIGH);

  uint16_t batteryVoltage = getBatteryVoltage();
  int16_t temperatura_ttn = temperature * 100;
   int16_t Humedad_ttn = humidity * 100;

  int16_t VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
  
  VRL = analogRead(ADC) * 100; //Measure the voltage drop and convert to 0-5V
//  Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
//  ratio = Rs/Ro;  // find ratio Rs/Ro
// 
//  float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm
  
    Serial.println( VRL);
   

  
  appDataSize = 6;
  appData[0] = (uint8_t)(temperatura_ttn >> 8);
  appData[1] = (uint8_t)temperatura_ttn;
  appData[2] = (uint8_t)(Humedad_ttn >> 8);
  appData[3] = (uint8_t)Humedad_ttn;

  appData[4] = (uint8_t)( VRL >> 8);
  appData[5] = (uint8_t) VRL;

  appData[6] = (uint8_t)(batteryVoltage >> 8);
  appData[7] = (uint8_t)batteryVoltage;

}


void setup() {
  
  Serial.begin(9600);
#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();
}

void loop()
{
   
  switch ( deviceState )
  {
    case DEVICE_STATE_INIT:
      {

        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        LoRaWAN.join();
        
        break;
      }
    case DEVICE_STATE_SEND:
      {
        Serial.println("pre");
        prepareTxFrame( appPort );
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        LoRaWAN.sleep();
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}

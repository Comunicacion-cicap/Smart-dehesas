
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>

#include "softSerial.h"

unsigned char data[4] = {};
float distance;

softSerial mySerial(SCL /*TX pin*/, SDA /*RX pin*/); // RX, TX

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
  delay(5000);

  do {
    for (int i = 0; i < 4; i++)
    {
      data[i] = softSerial.read();
    }
  } while (softSerial.read() == 0xff);

  softSerial.flush();

  digitalWrite(Vext, HIGH);

  if (data[0] == 0xff)
  {
    int sum;
    sum = (data[0] + data[1] + data[2]) & 0x00FF;
    if (sum == data[3])
    {
      distance = 5;
      distance = (data[1] << 8) + data[2];
      if (distance > 280)
      {
        Serial.print("distance=");
        Serial.print(distance);
        Serial.println("cm");
      } else
      {
        Serial.println("Below the lower limit");
      }
    }
  }


  uint16_t batteryVoltage = getBatteryVoltage();
  int16_t distancia_ttn = distance * 100;


  appDataSize = 4;
  appData[0] = (uint8_t)(distancia_ttn >> 8);
  appData[1] = (uint8_t)distancia_ttn;
  appData[2] = (uint8_t)(batteryVoltage >> 8);
  appData[3] = (uint8_t)batteryVoltage;


}


void setup() {

  Serial.begin(9600);
 
  softwareSerial.begin(9600);

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

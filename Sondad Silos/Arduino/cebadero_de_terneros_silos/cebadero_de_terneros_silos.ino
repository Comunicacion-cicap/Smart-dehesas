#include <MKRWAN.h>
#include <WDTZero.h>
#include <Wire.h>
#include "ADS1X15.h"
#include "config.h"
#define ADS1115_CONVERSIONDELAY         (1)
#define ID 10


WDTZero MyWatchDoggy; // Define WDT
LoRaModem modem;
ADS1115 ads(0x48);

String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

int contador_Motor = 1000;
int contador_envio_lora = 3;

unsigned long TimeReloj = millis();
unsigned long TimeReloj_lora = millis();
unsigned long TimeReloj_Motor = millis();


int estado = 0;

const float FACTOR = 30; //30A/1V
const float multiplier = 0.0625F;

bool Recibido_1 = 0;
bool Recibido_2 = 0;

String Ultimo_dato;
int Silo_1 = 0;
int Silo_2 = 0;

int tiempo_ejecucion_1 = 0;
int tiempo_ejecucion_2 = 0;

int Evento = 0;

int contador_mensaje = 0;

void setup_perroguardian() {

  MyWatchDoggy.attachShutdown(myshutdown);
  MyWatchDoggy.setup(WDT_SOFTCYCLE1M);  // initialize WDT-soft //WDT_SOFTCYCLE1M // WDT_SOFTCYCLE32S

}

void myshutdown()
{
  Serial.print("\nWe gonna shut down ! ...");
}


void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);
//
  ads.setGain(0);
  ads.begin();

  TimeReloj = millis();
  TimeReloj_lora = millis();
  TimeReloj_Motor = millis();
  setup_perroguardian();
  if (!modem.begin(EU868)) {

    Serial.println("Fallo al iniciar modulo!!");
   // while (1) {}
  };

 

 //int connected = modem.joinOTAA(appEui,appKey);
  int connected = modem.joinABP(devAddr, nwkSKey, appSKey);

  if (!connected) {
    Serial.println("Error al intentar conectarse a TTN. Posiblemente no tengas cobertura!!");
  //  while (1) {}
  }

  modem.setADR(false);
  modem.dataRate(0);
  modem.minPollInterval(60);

  perroguardian();
 
}
void perroguardian() {
  MyWatchDoggy.clear();
}

void loop(void) {

 
  perroguardian();
  if ((millis() - TimeReloj) > contador_envio_lora ) {


    switch (estado) {
      case 0:

        Serial.write("S01");
        Ultimo_dato = "S01";
        estado = 4;
        contador_mensaje = 0;
        Recibido_1 = 0;
        break;

      case 1:

        if (Recibido_1 == 1 ) {
          estado = 4;
        }

        if (contador_mensaje  > 250000) {
          estado = 0;
        }
        contador_mensaje++;
        break;
      case 2:
        Serial.write("S02");

        Ultimo_dato = "S02";
        estado = 3;
        contador_mensaje = 0;
        Recibido_2 = 0;
        break;
      case 3:
        if ( Recibido_2 == 1) {
          estado = 4;
        }
        if (contador_mensaje  > 250000) {
          estado = 2;
        }
        contador_mensaje++;
        break;
      case 4:



        int16_t tiempo_ejecucion_1 = tiempo_ejecucion_1 ;
        int16_t tiempo_ejecucion_2 = tiempo_ejecucion_2 ;


        // int16_t tiempo_ejecucion_1 = random(1, 100);
        // int16_t tiempo_ejecucion_2 = random(1, 100);

        int16_t Silo1_1 = Silo_1;
        int16_t Silo1_2 = Silo_2;

        int16_t Evento = 0;

        byte buffer[10];
        buffer[0] = Silo1_1 >> 8;
        buffer[1] = Silo1_1;
        buffer[2] = Silo1_2 >> 8;
        buffer[3] = Silo1_2;

        buffer[4] = tiempo_ejecucion_1 >> 8;
        buffer[5] = tiempo_ejecucion_1;
        buffer[6] = tiempo_ejecucion_2 >> 8;
        buffer[7] = tiempo_ejecucion_2;
        buffer[8] = Evento >> 8;
        buffer[9] = Evento;
        int err;

        modem.beginPacket();
        modem.write(buffer, sizeof(buffer));

        modem.endPacket(false);

        TimeReloj = millis();
        estado = 0;
        tiempo_ejecucion_1 = 0;
        tiempo_ejecucion_2 = 0;
        perroguardian();
        break;

    }
  }

  if ((millis() - TimeReloj_Motor) > contador_Motor) {

    float currentRMS_1 = getCorriente_0_1();
    float currentRMS_2 = getCorriente_2_3();

    if (currentRMS_1 > 0.1) {
      tiempo_ejecucion_1++;
    }

    if (currentRMS_2 > 0.1) {
      tiempo_ejecucion_2++;
    }

    TimeReloj_Motor = millis();
  }


   while (Serial.available() > 0) {
 
    String datatext = Serial.readString();

    Serial.println(datatext);
    datatext.replace("\r\n", "");
    datatext.replace("K", "G");
    datatext.replace("G", "");
    datatext.replace(" ", "");
    Serial.println(datatext.toInt());

    if (datatext.toInt() != 0) {
      if (Ultimo_dato == "S01") {

        Silo_1 = datatext.toInt();
        Recibido_1 = 1;
      }
      if (Ultimo_dato == "S02") {

        Silo_2 = datatext.toInt();
        Recibido_2 = 1;
      }
    }
  }
}


float getCorriente_0_1()
{
  float voltage;
  float corriente;
  float sum = 0;
  long tiempo = millis();
  int counter = 0;

  while (millis() - tiempo < 1000)
  {
    voltage = ads.readADC_Differential_0_1() * multiplier;
    corriente = voltage * FACTOR;
    corriente /= 1000.0;

    sum += sq(corriente);
    counter = counter + 1;
  }

  corriente = sqrt(sum / counter);
  return (corriente);
}


float getCorriente_2_3()
{
  float voltage;
  float corriente;
  float sum = 0;
  long tiempo = millis();
  int counter = 0;

  while (millis() - tiempo < 1000)
  {
    voltage = ads.readADC_Differential_2_3() * multiplier;
    corriente = voltage * FACTOR;
    corriente /= 1000.0;

    sum += sq(corriente);
    counter = counter + 1;
  }

  corriente = sqrt(sum / counter);
  return (corriente);
}

/*PAYLOAD TTN
  function Decoder(bytes, port) {
  var temperatura = (bytes[0] << 8) | bytes[1];
  var co2 = (bytes[2] << 8) | bytes[3];
  var tvoc = (bytes[4] << 8) | bytes[5];
  var humedad = (bytes[6] << 8) | bytes[7];
  var adc0 = (bytes[8] << 8) | bytes[9];
  var adc1 = (bytes[10] << 8) | bytes[11];
  var adc2 = (bytes[12] << 8) | bytes[13];
  var velocidadViento = 0;

  if(adc0 > 12){
    var tension = adc0 * 0.1875;
    velocidadViento = ((tension * 162)/2500);
  }

  //Calcular ppm para Metano

        var voltajeFuente = 5.076;
        var valorGases = adc2;
        var voltaje = (valorGases * 0.1880) / 1000;
        var Rs = 1000 * ((voltajeFuente - voltaje) / voltaje);
        var metano = 4443.1 * Math.pow((Rs / 9.6333), -2.583);

  //Calcular ppm para Amoniaco

        var voltajeFuente2 = 5.071;
        var valorAmoniaco = adc1;
        var voltaje2 = (valorAmoniaco * 0.1880) / 1000;
        var Rs2 = 1000 * ((voltajeFuente2 - voltaje2) / voltaje2);
        var amoniaco = 40.487 * Math.pow((Rs2/1247.701),-3.337);


  var decoded = {};

  if(temperatura / 100 > 100)
  {
    decoded.temperatura = 0;
  }else{
    decoded.temperatura = temperatura / 100;
  }

   if(humedad / 100 > 100)
  {
    decoded.humedad = 0;
  }else{
    decoded.humedad = humedad / 100;
  }

  //decoded.velocidadViento = velocidadViento / 100;
  decoded.co2 = co2;
  decoded.tvoc = tvoc;
  decoded.velocidadViento = velocidadViento;
  decoded.voltajeAnemometro = adc0;
  decoded.voltajeAmoniaco = adc1;
  decoded.voltajeGases = adc2;
  decoded.ppmMetano = metano;
  decoded.ppmAmoniaco = amoniaco;
  return decoded;
  }

*/

#include <MKRWAN.h>
#include <ArduinoLowPower.h>
#include "Arduino.h"
#include <WDTZero.h>
#include <Wire.h>
#include <DFRobot_SHT3x.h>
#include "ADS1X15.h"
#include "I2CScanner.h"


I2CScanner scanner;
LoRaModem modem;
WDTZero MyWatchDoggy;
DFRobot_SHT3x sht3x(&Wire,/*address=*/0x44,/*RST=*/4);
ADS1115 ads(0x48);


const char *devAddr = "260BD74B";
const char *nwkSKey = "48BC4BB58FA8CEDF3EA4908BE665CA9F";
const char *appSKey = "480D81C501798565227A965F78D9B58B";

unsigned long Reset = 0UL;
unsigned long tiempo_reset  = 300UL;
unsigned long TimeRX = 5UL * 60000UL; // prev es de tipo unsigned long
unsigned long prev; // prev es de tipo unsigned long

int connected;
byte buffer[16];

int16_t adc0; //ENTRADA ANEMOMETRO
int16_t adc1; //ENTRADA AMONIACO
int16_t adc2; //ENTRADA METANO
int16_t adc3; //ENTRADA Co2

int co2;
int tvoc;
float temperatura;
float humedad;
float temperature ;
float humidity ;

int16_t temperatura_ttn ;
int16_t humedad_ttn ;
int16_t co2_ttn ;
int16_t tvoc_ttn;

int16_t adc0_ttn ;
int16_t adc1_ttn ;
int16_t adc2_ttn;
int16_t adc3_ttn ;

bool results;
int option;




void myshutdown()
{
  Serial.print("\nWe gonna shut down ! ...");
}

void setup_perroguardian() {

  MyWatchDoggy.attachShutdown(myshutdown);
  MyWatchDoggy.setup(WDT_SOFTCYCLE1M);  // initialize WDT-soft //WDT_SOFTCYCLE1M // WDT_SOFTCYCLE32S

}
void perroguardian() {
  MyWatchDoggy.clear();
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  //while (!Serial);
  Serial.begin(115200);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  Serial.println("Inicio");

  setup_perroguardian();

  sht3x.begin();
  Serial.println("Modulo");

  if (!modem.begin(EU868)) {

    Serial.println("Fallo al iniciar modulo!!");
    while (1) {}
  };

  modem.setADR(false);

  /*
    DataRate  Modulation  SF  BW  bit/s
    0   LoRa  12  125   250
    1   LoRa  11  125   440
    2   LoRa  10  125   980
    3   LoRa  9   125   1'760
    4   LoRa  8   125   3'125
    5   LoRa  7   125   5'470
    6   LoRa  7   250   11'000
  */

  modem.dataRate(0);
  modem.minPollInterval(60);

  connected = modem.joinABP(devAddr, nwkSKey, appSKey);


  if (!connected) {
    Serial.println("Error al intentar conectarse a TTN. Posiblemente no tengas cobertura!!");
    //while (1) {}
  }

  delay(100);
  Serial.println("ads");
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  scanner.Init();

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  prev = millis();
}




void Toma_datos() {

  humidity = sht3x.getHumidityRH();
  temperature = sht3x.getTemperatureC();
  results = scanner.Check(0x48);
  Serial.println(results);
  if (results) {
    ads.setGain(0);
    adc0 = ads.readADC(0);
    adc1 = ads.readADC(1);
    adc2 = ads.readADC(2);
    adc3 = ads.readADC(3);

  } else {
    Serial.println("Failed to  ADS.");
    while (1);
  }

  temperatura_ttn = temperature * 100;
  humedad_ttn = humidity * 100;
  co2_ttn = co2;
  tvoc_ttn = tvoc;

  adc0_ttn = adc0;
  adc1_ttn = adc1;
  adc2_ttn = adc2;
  adc3_ttn = adc3;

}


void loop() {

  if ( millis() - prev > TimeRX ) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

    prev = millis();
    perroguardian();

    Toma_datos();

    buffer[0] = temperatura_ttn >> 8;
    buffer[1] = temperatura_ttn;
    buffer[2] = co2_ttn >> 8;
    buffer[3] = co2_ttn;
    buffer[4] = tvoc_ttn >> 8;
    buffer[5] = tvoc_ttn;
    buffer[6] = humedad_ttn >> 8;
    buffer[7] = humedad_ttn;
    buffer[8] = adc0_ttn >> 8;
    buffer[9] = adc0_ttn;
    buffer[10] = adc1_ttn >> 8;
    buffer[11] = adc1_ttn;
    buffer[12] = adc2_ttn >> 8;
    buffer[13] = adc2_ttn;
    buffer[14] = adc3_ttn >> 8;
    buffer[15] = adc3_ttn;

    modem.setPort(3);
    modem.beginPacket();
    modem.write(buffer, sizeof(buffer));
    modem.endPacket(false);

    Reset ++;

  } else {

    digitalWrite(LED_BUILTIN, LOW);
    perroguardian();

    if (Serial.available() > 0) {

      option = Serial.read();
      if (option == 'd') {

        Toma_datos();


        float voltajeFuente2 = 5.000;
        float valorAmoniaco = adc3;
        float voltaje2 = (valorAmoniaco * 0.1880) / 1000;
        float Rs2 = 1000 * ((voltajeFuente2 - voltaje2) / voltaje2);
        float amoniaco = 40.487 * pow((Rs2 / 1247.701), -3.337);

        Serial.print("Valor anemometro : "); Serial.println(adc0);
        Serial.print("Valor amoníaco : "); Serial.println(adc1);
        Serial.print("Valor amoníaco ppm : "); Serial.println(amoniaco);
        Serial.print("Valor metano : "); Serial.println(adc2);
        Serial.print("co2 : "); Serial.println(adc3);
        Serial.print("Temperatura : "); Serial.println(temperatura);
        Serial.print("Humedad : "); Serial.println(humedad);
        Serial.print("Co2 : "); Serial.println(co2_ttn);
        Serial.print("tvoc_ttn : "); Serial.println(tvoc_ttn);
        Serial.println("");
      }
    }
  }

  if (Reset > tiempo_reset) {
    //while (1) {}
    Reset = 0;
  }
}

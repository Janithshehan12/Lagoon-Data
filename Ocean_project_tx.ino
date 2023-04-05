#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3

#define sensor_pin A1
int read_ADC;
int ntu;

#define ss 10
#define rst 9
#define dio0 2

float waterTemp = 0;

String Mymessage = "";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemperature(&oneWire);
DeviceAddress insideThermometer;

void setup() {
  Serial.begin(9600);
  Serial.print("Locating devices...");
  dallasTemperature.begin();
  Serial.print("Found ");
  Serial.print(dallasTemperature.getDeviceCount(), DEC);
  Serial.println(" devices.");

  
  Serial.print("Parasite power is: "); 
  if (dallasTemperature.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!dallasTemperature.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution 
  dallasTemperature.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(dallasTemperature.getResolution(insideThermometer), DEC); 
  Serial.println();

  pinMode(sensor_pin, INPUT);

  while (!Serial);

  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(10);
}

void loop() {
  Serial.print("Sending packet: ");
  LoRa.beginPacket();

  tb();
  dallasTemperature.requestTemperatures();
  waterTemp = dallasTemperature.getTempCByIndex(0);
  Mymessage = Mymessage + waterTemp + "," + ntu;
  Serial.print (" ");
  Serial.println (Mymessage);
  LoRa.print(Mymessage);
  LoRa.endPacket();
  delay(500);
  Mymessage = " ";
}

  void tb() {
  read_ADC = analogRead(sensor_pin);
  if (read_ADC > 208)read_ADC = 208;

  ntu = map(read_ADC, 0, 208, 300, 0);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

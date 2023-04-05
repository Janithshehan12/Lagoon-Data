#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <LoRa.h>

// password and ID for open networks.
char ssid[] = "JanithOcean";
char pass[] = "ocean12345";
//  Auth Token are provided by the Blynk.Cloud
char auth[] = "yy1XOXlyDiw7h6gAaZzN-vmXo99sgg8h";

#define ss 5
#define rst 14
#define dio0 2

float sensor1;
float sensor2;

String incoming = "";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  while (!Serial);

  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(10);

}

void loop() {

  Blynk.run();

  unsigned long now = millis();
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    //Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      //Serial.print((char)LoRa.read());
      incoming += (char)LoRa.read();
    }

    String q = getValue(incoming, ',', 0);
    String r = getValue(incoming, ',', 1);
    Serial.println("Received packet : ");
    Serial.print(q);
    Serial.print(" , ");
    Serial.println(r);

    sensor1 = q.toFloat();
    sensor2 = r.toFloat();
    Blynk.virtualWrite(V1, sensor1);
    Blynk.virtualWrite(V2, sensor2);
  }
  incoming = "";
  delay(10);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

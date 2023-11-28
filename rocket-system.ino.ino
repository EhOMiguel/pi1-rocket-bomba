// #include "gps.h"

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define RX_PIN_GPS 17
#define TX_PIN_GPS 16
#define CS_SD_CARD 5
#define CLK_SD_CARD 18
#define MOSI_SD_CARD 23
#define MISO_SD_CARD 19


WiFiServer server(80);
const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

float flat = 0.0f, flon = 0.0f;
float speed;
unsigned long age;
byte month, hour, day, minute, second;
int year;
char dt[32];

TinyGPS gps;
SoftwareSerial gpsSerial(16, 17);

void setup()
{
  // Start Serial
  gpsSerial.begin(9600);
  Serial.begin(9600);

  Serial.println("Starting....");

  // Configura a ESP32 como Access Point
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Iniciado");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client) {
    Serial.print("New client available...");

    // Agora, envie os dados do GPS para o cliente
    if (client.connected()) {

      // client.print("HTTP/1.1 200 OK\r\nContent-Type: text/text\r\n\r\n");
      client.print("LAT=");
      client.print(flat, 6);
      client.print(" LON=");
      client.print(flon, 6);
      client.print(" SPEED=");
      client.print(speed);
      client.print(" DATETIME=");
      client.println(dt);
    }
  }
  read_gps_data();

  // write_data_to_sd_card();

  // delay(1000);
}

void read_gps_data() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // if (gps.location.isValid()) {
      gps.f_get_position(&flat, &flon, &age);
      speed = gps.f_speed_mps() * 3.6;
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);
      
      sprintf(dt, "%02d/%02d/%02d %02d:%02d:%02d", day, month, year, hour-3, minute, second);

      Serial.print("LAT=");
      Serial.print(flat, 6);
      Serial.print(" LON=");
      Serial.print(flon, 6);
      Serial.print(" SPEED=");
      Serial.print(speed);
      Serial.print(" DATETIME=");
      Serial.println(dt);
    }
  }
  
  // gps.stats(&chars, &sentences, &failed);
}
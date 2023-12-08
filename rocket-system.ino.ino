// #include "gps.h"

#include <SoftwareSerial.h>
#include <TinyGPS.h>
// #include <SD.h>
// #include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define RX_PIN_GPS 17
#define TX_PIN_GPS 16
#define CS_SD_CARD 5
#define CLK_SD_CARD 18
#define MOSI_SD_CARD 23
#define MISO_SD_CARD 19

const unsigned char UBLOX_INIT[] PROGMEM =
{
  //// Rate (pick one)
  // 0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  // 0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39, //(1Hz)
  //// Disable specific NMEA sentences
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
  //0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47 // GxVTG off

  // Rate
  // 0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12,   // (10Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, // (5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39  // (1Hz)
};


WiFiServer server(2503);
const char *ssid = "LaBamba";             // SSID para o Wifi da ESP32
const char *password = "123456789";       // Senha para a rede Wifi da ESP32

char formatted_string[150];
float flat = 0.0f, flon = 0.0f;
long altitude;
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
  IPAddress IP = WiFi.softAPIP();

  Serial.println("Wifi Iniciado");
  Serial.print("IP Address: ");
  Serial.println(IP);

  server.begin();

  Serial.write("---------------------- GPS CONFIGURATION START --------------------------\n");
  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {
    Serial.write("Enviando dado...\n");
    gpsSerial.write( pgm_read_byte(UBLOX_INIT+i) );
  };
  Serial.write("-------------------------- GPS RECEPTION FINISH --------------------------\n");
}

void loop()
{
  WiFiClient client = server.available();

  if (client) {
    // Serial.print("Novo cliente disponível...");

    // Agora, envie os dados do GPS para o cliente
    if (client.connected()) {
      // Serial.println("Cliente conectado!");

      // client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
      client.println(formatted_string);
      
      // delay(100);
    }
  }

  read_gps_data();

  // write_data_to_sd_card();

  // delay(200);
  client.stop();
  // Serial.println("Cliente desconectado!");
}

void read_gps_data() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&flat, &flon, &age);

      speed = gps.f_speed_mps();
      altitude = gps.altitude();
      
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);
      
      sprintf(dt, "%02d/%02d/%02d %02d:%02d:%02d", day, month, year, hour-3, minute, second);
      sprintf(formatted_string, "LAT=%.6f;LON=%.6f;ALTITUDE=%ld;SPEED=%.2f;DATETIME=%s", flat, flon, altitude, speed, dt);

      Serial.println(formatted_string);
    }
  }
}
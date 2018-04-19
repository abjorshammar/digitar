
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <GxEPD.h>
#include <GxGDEW0154Z04/GxGDEW0154Z04.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "qrcode.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include <WiFiClientSecure.h>


// Define pins
int chargePin = D1;
int battPin = A0;
int battery = 5; // 0 = empty, 2 = 25%, 5 = 50%, 7 = 75%, 10 = 100%, 100 = charging

GxIO_Class io(SPI, SS, 0, 2);
GxEPD_Class display(io);

WiFiClientSecure client;

static const uint8_t PROGMEM wifi_inv [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xfc, 0xff, 0xff, 0x07, 0x3f, 0x00,
  0x00, 0x62, 0x40, 0x08, 0x18, 0xf1, 0x01, 0xfc, 0x36, 0xd9, 0x03, 0x3e,
  0x00, 0x00, 0xfe, 0xe0, 0x1f, 0xf8, 0xf1, 0x03, 0x7c, 0x00, 0x00, 0x03,
  0x3e, 0x00, 0x00, 0xfe, 0xe0, 0x1f, 0xfc, 0xf1, 0x03, 0x7c, 0x00, 0x80,
  0x03, 0x3e, 0x00, 0x00, 0xfe, 0xe0, 0x1f, 0xfc, 0xf1, 0x03, 0x7e, 0x00,
  0x00, 0x03, 0x3e, 0x00, 0x00, 0xfc, 0xe0, 0x1f, 0xfc, 0xf1, 0x01, 0x7e,
  0x00, 0x00, 0x07, 0x3f, 0x00, 0x00, 0xfc, 0xe0, 0x3f, 0xfc, 0x01, 0x00,
  0x7c, 0xe0, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xfc, 0xe0, 0x3f, 0xfc, 0x00,
  0x00, 0x7e, 0xe0, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xf8, 0xf1, 0x3f, 0xfc,
  0xf0, 0x03, 0x7e, 0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xf8, 0xf1, 0x3f,
  0xfc, 0xf0, 0x01, 0x7c, 0xc0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xf8, 0xf1,
  0x7c, 0x7e, 0xf0, 0x01, 0x7c, 0x00, 0xea, 0x03, 0x3e, 0x00, 0x00, 0xf8,
  0xfb, 0x7d, 0x7e, 0xf0, 0x03, 0x7e, 0x00, 0xe0, 0x03, 0x3e, 0x00, 0x00,
  0xf8, 0xfb, 0x7c, 0x7e, 0xf0, 0x01, 0x7c, 0x00, 0xe0, 0x03, 0x3e, 0x00,
  0x00, 0xf0, 0xfb, 0x7c, 0x7e, 0xf0, 0x01, 0x7c, 0x00, 0xe0, 0x03, 0x3e,
  0x00, 0x00, 0xf0, 0xfb, 0x7c, 0x7e, 0xf0, 0x03, 0x7e, 0x00, 0xe0, 0x03,
  0x3e, 0x00, 0x00, 0xf0, 0xfb, 0x7c, 0x7f, 0xf0, 0x01, 0x7c, 0x40, 0xff,
  0x03, 0x3e, 0x00, 0x00, 0xf0, 0xff, 0xfc, 0x3e, 0xf0, 0x01, 0x7e, 0xc0,
  0xff, 0x03, 0x3e, 0x00, 0x00, 0xe0, 0xff, 0xf8, 0x3f, 0xf0, 0x03, 0x7c,
  0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xe0, 0x7f, 0xf8, 0x3f, 0xf0, 0x03,
  0x7c, 0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xe0, 0xff, 0xf8, 0x1f, 0xf0,
  0x01, 0x7c, 0xc0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xc0, 0x7f, 0xf0, 0x1f,
  0xf0, 0x03, 0x7c, 0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xc0, 0x7f, 0xf0,
  0x1f, 0xf0, 0x03, 0x7c, 0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xc0, 0x3f,
  0xf0, 0x0f, 0xf0, 0x01, 0x7e, 0xc0, 0xff, 0x03, 0x3e, 0x00, 0x00, 0xc0,
  0x3f, 0xe0, 0x0f, 0xf0, 0x01, 0x7e, 0xe0, 0xff, 0x03, 0x3e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
  0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff,
  0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff,
  0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

static const uint8_t PROGMEM wifi [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x03, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x07, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x1f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x3f, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0xfc, 0x01, 0xf8, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xfc, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf0, 0x03, 0xfe, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xe0, 0x07, 0xfe,
  0xff, 0xff, 0xff, 0xff, 0x0f, 0xfe, 0x03, 0x00, 0x00, 0xf8, 0xc0, 0x07,
  0xfe, 0x9d, 0xbf, 0xf7, 0xe7, 0x0e, 0xfe, 0x03, 0xc9, 0x26, 0xfc, 0xc1,
  0x07, 0xfe, 0x01, 0x1f, 0xe0, 0x07, 0x0e, 0xfc, 0x83, 0xff, 0xff, 0xfc,
  0xc1, 0x07, 0xfe, 0x01, 0x1f, 0xe0, 0x03, 0x0e, 0xfc, 0x83, 0xff, 0x7f,
  0xfc, 0xc1, 0x07, 0xfe, 0x01, 0x1f, 0xe0, 0x03, 0x0e, 0xfc, 0x81, 0xff,
  0xff, 0xfc, 0xc1, 0x07, 0xfe, 0x03, 0x1f, 0xe0, 0x03, 0x0e, 0xfe, 0x81,
  0xff, 0xff, 0xf8, 0xc0, 0x07, 0xfe, 0x03, 0x1f, 0xc0, 0x03, 0xfe, 0xff,
  0x83, 0x1f, 0x00, 0x00, 0xc0, 0x07, 0xfe, 0x03, 0x1f, 0xc0, 0x03, 0xff,
  0xff, 0x81, 0x1f, 0x00, 0x00, 0xc0, 0x07, 0xfe, 0x07, 0x0e, 0xc0, 0x03,
  0x0f, 0xfc, 0x81, 0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x07, 0x0e, 0xc0,
  0x03, 0x0f, 0xfe, 0x83, 0x3f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x07, 0x0e,
  0x83, 0x81, 0x0f, 0xfe, 0x83, 0xff, 0x15, 0xfc, 0xc1, 0x07, 0xfe, 0x07,
  0x04, 0x82, 0x81, 0x0f, 0xfc, 0x81, 0xff, 0x1f, 0xfc, 0xc1, 0x07, 0xfe,
  0x07, 0x04, 0x83, 0x81, 0x0f, 0xfe, 0x83, 0xff, 0x1f, 0xfc, 0xc1, 0x07,
  0xfe, 0x0f, 0x04, 0x83, 0x81, 0x0f, 0xfe, 0x83, 0xff, 0x1f, 0xfc, 0xc1,
  0x07, 0xfe, 0x0f, 0x04, 0x83, 0x81, 0x0f, 0xfc, 0x81, 0xff, 0x1f, 0xfc,
  0xc1, 0x07, 0xfe, 0x0f, 0x04, 0x83, 0x80, 0x0f, 0xfe, 0x83, 0xbf, 0x00,
  0xfc, 0xc1, 0x07, 0xfe, 0x0f, 0x00, 0x03, 0xc1, 0x0f, 0xfe, 0x81, 0x3f,
  0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x1f, 0x00, 0x07, 0xc0, 0x0f, 0xfc, 0x83,
  0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x1f, 0x80, 0x07, 0xc0, 0x0f, 0xfc,
  0x83, 0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x1f, 0x00, 0x07, 0xe0, 0x0f,
  0xfe, 0x83, 0x3f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x3f, 0x80, 0x0f, 0xe0,
  0x0f, 0xfc, 0x83, 0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x3f, 0x80, 0x0f,
  0xe0, 0x0f, 0xfc, 0x83, 0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x3f, 0xc0,
  0x0f, 0xf0, 0x0f, 0xfe, 0x81, 0x3f, 0x00, 0xfc, 0xc1, 0x07, 0xfe, 0x3f,
  0xc0, 0x1f, 0xf0, 0x0f, 0xfe, 0x81, 0x1f, 0x00, 0xfc, 0xc1, 0x07, 0xfe,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xc0, 0x07,
  0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0x03, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
  0x00, 0xf0, 0x03, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00,
  0x00, 0x00, 0xf8, 0x01, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00,
  0x00, 0x00, 0x00, 0xfe, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0xf8,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

// Envelope
static const uint8_t PROGMEM envelope[] = {
  0xfc, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0x03, 0x57, 0x55, 0xd5, 0x03,
  0x07, 0x00, 0x00, 0x03, 0x03, 0x00, 0x80, 0x03, 0x07, 0x00, 0x80, 0x03,
  0x1f, 0x00, 0xc0, 0x03, 0x3f, 0x00, 0xf0, 0x03, 0x77, 0x00, 0xb8, 0x03,
  0xe3, 0x00, 0x1c, 0x03, 0xc7, 0x03, 0x8f, 0x03, 0x83, 0x87, 0x07, 0x03,
  0x07, 0xef, 0x83, 0x03, 0x03, 0xfc, 0x00, 0x03, 0x07, 0x38, 0x80, 0x03,
  0x03, 0x00, 0x00, 0x03, 0x07, 0x00, 0x80, 0x03, 0xaf, 0xaa, 0xaa, 0x03,
  0xfe, 0xff, 0xff, 0x03, 0xfc, 0xff, 0xff, 0x00 };

// Home office
static const uint8_t PROGMEM homeoffice[] = {
  0x00, 0x00, 0x80, 0x00, 0xd8, 0x00, 0x00, 0x00, 0xe0, 0x07, 0xfc, 0x00,
  0x00, 0x00, 0xf0, 0x0f, 0xfc, 0x01, 0x00, 0x00, 0xf8, 0x1f, 0xfc, 0x00,
  0x00, 0x00, 0xfc, 0x3f, 0xfc, 0x01, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00,
  0x00, 0x80, 0xff, 0xfe, 0xfd, 0x01, 0x00, 0xc0, 0x3f, 0xfc, 0xff, 0x00,
  0x00, 0xe0, 0x1f, 0xf8, 0xff, 0x01, 0x00, 0xf0, 0x0f, 0xf0, 0xff, 0x00,
  0x00, 0xfc, 0x83, 0xc1, 0xff, 0x01, 0x00, 0xfe, 0xc3, 0x83, 0xff, 0x00,
  0x00, 0xff, 0xf0, 0x0f, 0xff, 0x01, 0x80, 0x7f, 0xf8, 0x1f, 0xfe, 0x01,
  0xc0, 0x3f, 0xfc, 0x3f, 0xfc, 0x03, 0xf0, 0x1f, 0xfe, 0x7f, 0xf8, 0x0f,
  0xf8, 0x07, 0xff, 0xff, 0xe0, 0x1f, 0xfc, 0xc3, 0xff, 0xff, 0xc3, 0x3f,
  0xfe, 0xe1, 0xff, 0xff, 0x87, 0x7f, 0xff, 0xf0, 0xff, 0xff, 0x0f, 0xff,
  0x7e, 0xf8, 0xff, 0xff, 0x1f, 0x7e, 0x1c, 0xfc, 0xff, 0xff, 0x3f, 0x38,
  0x0c, 0xff, 0xff, 0xff, 0xff, 0x30, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x80, 0xff, 0xff, 0xff, 0xff, 0x01, 0x80, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x00, 0xff, 0x9f, 0xf4, 0xff, 0x01, 0x80, 0xff, 0x0f, 0xf0, 0xff, 0x00,
  0x80, 0xff, 0x0f, 0xf0, 0xff, 0x01, 0x00, 0xff, 0x0f, 0xf0, 0xff, 0x00,
  0x80, 0xff, 0x0f, 0xf0, 0xff, 0x01, 0x80, 0xff, 0x0f, 0xf0, 0xff, 0x00,
  0x00, 0xff, 0x0f, 0xf0, 0xff, 0x01, 0x80, 0xff, 0x0f, 0xf0, 0xff, 0x00,
  0x80, 0xff, 0x0f, 0xf0, 0xff, 0x01, 0x00, 0xff, 0x0f, 0xf0, 0xff, 0x00,
  0x80, 0xff, 0x0f, 0xf0, 0xff, 0x00, 0x00, 0xfe, 0x0f, 0xf0, 0xff, 0x00
  };

static const uint8_t PROGMEM battery_charging[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xef, 0x03, 0xc0, 0xef, 0x03, 0xc0, 0xe7, 0x03, 0xc0, 0xe7, 0x03,
  0xc0, 0xe3, 0x03, 0xc0, 0xc3, 0x03, 0xc0, 0xc3, 0x03, 0xc0, 0xe7, 0x03,
  0xc0, 0xe7, 0x03, 0xc0, 0xf7, 0x03, 0xc0, 0xf7, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM battery_full[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM battery_75[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM battery_50[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM battery_25[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM battery_empty[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03,
  0xc0, 0xff, 0x03, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM grumpy[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
  0xff, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0xff, 0xff, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xe0, 0xff, 0x7f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x00, 0xff, 0x3f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x7f, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x7f, 0x00, 0xfe, 0xff, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0xfc, 0xff,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x3f, 0x00, 0xfc,
  0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xff, 0xff, 0x3f, 0x00,
  0xf8, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f,
  0x00, 0xf8, 0xff, 0xff, 0x0b, 0x0e, 0x00, 0x00, 0x00, 0xe8, 0xff, 0xff,
  0x1f, 0x00, 0xf0, 0xff, 0xff, 0xcf, 0x1f, 0x00, 0x00, 0x00, 0xc0, 0xff,
  0xff, 0x1f, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x80,
  0xff, 0xff, 0x17, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00,
  0x00, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xbf, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0x07, 0x00, 0x80, 0xff, 0xff, 0x3f, 0x00, 0x00,
  0x00, 0x00, 0x10, 0xff, 0xff, 0x03, 0x00, 0x00, 0xff, 0xff, 0x5f, 0x00,
  0x00, 0x00, 0x00, 0xb0, 0xff, 0xff, 0x03, 0x00, 0x00, 0xfe, 0xff, 0x9f,
  0x00, 0x00, 0x00, 0x00, 0xa8, 0xff, 0xff, 0x03, 0x00, 0x00, 0xfe, 0xff,
  0x9f, 0x05, 0x00, 0x00, 0x44, 0xf4, 0xff, 0xff, 0x03, 0x00, 0x00, 0xfe,
  0xff, 0x5f, 0x09, 0x44, 0x24, 0x14, 0xcd, 0xff, 0xff, 0x01, 0x00, 0x00,
  0xfe, 0xff, 0xff, 0x43, 0xc8, 0x44, 0x55, 0xbc, 0xff, 0xff, 0x01, 0x00,
  0x00, 0xf0, 0xff, 0x7f, 0x59, 0xca, 0x76, 0xb6, 0xdb, 0xff, 0xff, 0x00,
  0x00, 0x00, 0xf4, 0xff, 0x7f, 0x53, 0x43, 0xc9, 0x75, 0x5f, 0xff, 0xff,
  0x00, 0x00, 0x00, 0xf8, 0xff, 0x7f, 0xd5, 0xb7, 0x6d, 0xf7, 0xff, 0xff,
  0x7f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xab, 0xf7, 0xed, 0xef, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x57, 0xf5, 0xff, 0xfd, 0xf7,
  0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xb4, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xf2, 0x00, 0x00, 0x00, 0xf0, 0xbf, 0xd6, 0xfe, 0xff,
  0xff, 0xff, 0xff, 0x9b, 0x1a, 0x00, 0x00, 0x00, 0xe0, 0xdf, 0xd8, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x5f, 0x02, 0x00, 0x00, 0x00, 0xf0, 0x1f, 0xfb,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x6b, 0x6a, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x6f, 0xf5, 0x00, 0x00, 0x00, 0xc0,
  0x90, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfd, 0x03, 0x00, 0x00,
  0xe0, 0x25, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfa, 0x07, 0x00,
  0x00, 0xf0, 0xc9, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0x00, 0x00, 0xf0, 0xd3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
  0x0f, 0x00, 0x00, 0xf8, 0xeb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf7, 0x1f, 0x00, 0x00, 0xfc, 0xd6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x3f, 0x00, 0x00, 0xfe, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xdf, 0x7a, 0x00, 0x00, 0xfe, 0xfa, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x7b, 0x00, 0x00, 0x4e, 0xf5, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0x00, 0x00, 0x5e, 0xff, 0xff, 0xff,
  0xff, 0xeb, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x00, 0x00, 0x53, 0xff, 0xff,
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0x5e, 0x00, 0x00, 0xae, 0xfe,
  0xff, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x6a,
  0xff, 0xff, 0xff, 0xff, 0xd5, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x01, 0x00,
  0xfd, 0xff, 0xff, 0xff, 0xff, 0xc5, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00,
  0x00, 0xea, 0xff, 0xff, 0xff, 0xff, 0x91, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0x01, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff,
  0x7f, 0x00, 0x00, 0xf5, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff,
  0xff, 0x1f, 0x00, 0x00, 0xfa, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff,
  0xf8, 0xff, 0x7f, 0x00, 0x00, 0xf2, 0xff, 0x7f, 0xc6, 0xff, 0x00, 0xff,
  0x23, 0xf8, 0xff, 0x17, 0x00, 0x00, 0xf6, 0xff, 0x6f, 0x80, 0xff, 0x00,
  0xff, 0x00, 0xfd, 0xff, 0x37, 0x00, 0x00, 0xec, 0xff, 0x3f, 0xf0, 0xff,
  0x80, 0xff, 0x83, 0xfe, 0xff, 0x17, 0x00, 0x00, 0xf8, 0xff, 0x7f, 0xf1,
  0xff, 0x00, 0xff, 0x8f, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xd0, 0xff, 0xff,
  0xff, 0x7f, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0xd0, 0xff,
  0xff, 0xff, 0x7f, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xc0,
  0xff, 0xff, 0xff, 0x3f, 0x00, 0xfc, 0xff, 0xff, 0xff, 0x15, 0x00, 0x00,
  0x80, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x07, 0x00,
  0x00, 0x40, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x0d,
  0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xf0, 0xff, 0xff, 0xff,
  0x05, 0x00, 0x00, 0x20, 0xff, 0xff, 0xff, 0x03, 0x00, 0xe0, 0xff, 0xff,
  0xff, 0x02, 0x00, 0x00, 0x40, 0xff, 0xff, 0xff, 0x07, 0x00, 0xc0, 0xff,
  0xff, 0xdf, 0x03, 0x00, 0x00, 0x80, 0xff, 0xff, 0xbf, 0x00, 0x00, 0x02,
  0xff, 0xdf, 0xff, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x60, 0x81,
  0x02, 0xf4, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xfa, 0xef, 0x0b, 0xc0,
  0xeb, 0x01, 0xa8, 0xff, 0x2f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xbf, 0x07,
  0x00, 0x2a, 0x00, 0xc0, 0xff, 0x7b, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
  0x02, 0x00, 0x08, 0x00, 0xa0, 0xfe, 0x27, 0x00, 0x00, 0x00, 0x00, 0xf0,
  0xff, 0x02, 0x00, 0x00, 0x00, 0x40, 0xfd, 0x0d, 0x00, 0x00, 0x00, 0x00,
  0xd0, 0x1f, 0x05, 0x00, 0x08, 0x00, 0x00, 0x51, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xf5, 0x03, 0x00, 0x22, 0x00, 0x00, 0x7f, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x40, 0xbf, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x00, 0x00, 0x00, 0x80, 0x3a, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x02, 0x00, 0x00, 0x00, 0x40, 0x07,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x03, 0x00, 0x00, 0x00, 0x40,
  0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x04, 0x08, 0x00, 0x00,
  0xa0, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1e, 0x00, 0x00,
  0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00,
  0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00 };

static const uint8_t PROGMEM southpark[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91,
  0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x40, 0x04, 0x04, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x09, 0x00, 0x08, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x10, 0x08, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x09, 0x08, 0x81, 0x40, 0x02,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x08,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x10,
  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x22, 0x04, 0x00,
  0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x04, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x00, 0x00, 0x20, 0x08, 0x20, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
  0x04, 0x40, 0x48, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x04, 0x00, 0x20, 0x01, 0x02, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x20, 0x24, 0x40, 0x00, 0x00, 0x08, 0x80, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x24, 0x02, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xd5, 0x02, 0x40, 0x6a, 0x05, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x3f, 0x80, 0xff, 0x3f,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0xc0, 0xad, 0x2e, 0x62, 0x6b,
  0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x84, 0xff, 0x7d, 0x40,
  0xef, 0x3b, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xda, 0x37,
  0xc0, 0xbb, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x80, 0xb7,
  0x36, 0x42, 0xb7, 0x3a, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x48,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x00, 0x84, 0x48, 0xa0, 0x88, 0x84, 0x00, 0x04, 0x00, 0x00, 0x00, 0x80,
  0x00, 0xfc, 0xff, 0xff, 0xf9, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x08, 0x00, 0x00, 0x09, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01, 0x08, 0x00,
  0x00, 0x00, 0x10, 0x00, 0x0c, 0x00, 0x00, 0x09, 0x00, 0x00, 0x83, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x09, 0x00, 0x00,
  0x0b, 0x08, 0x00, 0x00, 0x00, 0x40, 0x82, 0x04, 0x00, 0x00, 0x09, 0x00,
  0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x09,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00,
  0x09, 0x00, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0xf8, 0x55, 0x09, 0x00,
  0x04, 0x09, 0x01, 0x00, 0x53, 0xf7, 0x00, 0x00, 0x00, 0x80, 0x6a, 0x07,
  0x00, 0x03, 0x8d, 0x02, 0x00, 0x6f, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x1e, 0x89, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x04, 0x00, 0x0b, 0x89, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x0a, 0x89, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x08, 0x04, 0x00, 0x00, 0x09, 0x00, 0x00, 0x21, 0x09, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x08, 0x00, 0x00, 0x09, 0x00, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x09, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x09, 0x00,
  0x00, 0x91, 0x04, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 0x00, 0x09,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00,
  0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0xff,
  0xff, 0xf9, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x24, 0x00, 0x50, 0x51, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x41, 0xfe, 0xf8, 0x43, 0x02, 0x88, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x84, 0x00, 0x04, 0xda, 0x70, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x77, 0xd1, 0x05, 0x20, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x80, 0xff, 0xff, 0x9f,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x01, 0x00,
  0x18, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x94, 0x24, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x04, 0xfc, 0xff, 0x81, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x40, 0x40, 0xf8, 0xff, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x06, 0x10, 0xf0, 0x3f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0f, 0x00, 0xe0, 0x3f, 0x40, 0x88, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x3f, 0x02, 0xc0, 0x0f, 0x00, 0xe0, 0x07, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x7f, 0x40, 0x80, 0x87, 0x84, 0xe0, 0x0f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x01, 0x00, 0x03, 0x00, 0xfe, 0x0f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x02, 0x00, 0x00, 0xff,
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x3f, 0x01, 0x05, 0xe9,
  0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff,
  0xfb, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x6f,
  0xdb, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff,
  0x7f, 0xbf, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
  0xdf, 0xf7, 0xf5, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,
  0xff, 0xff, 0xbf, 0xef, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfc, 0xff, 0xff, 0x5f, 0xe5, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xfc, 0xff, 0xff, 0x3f, 0xf0, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xfc, 0xff, 0xff, 0x7d, 0xf8, 0xfb, 0xff, 0xff, 0x01, 0x00,
  0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x78, 0xff, 0xff, 0xff, 0x01,
  0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff,
  0x01, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff,
  0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc5, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x1f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf8, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x44, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x40, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x10, 0x00, 0x00,
  0x00, 0x00, 0x40, 0x04, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x80, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x01, 0x00, 0x00, 0x00 };


void drawStatusWindow(const uint8_t *avatar, String ticket, int battery) {
  display.fillScreen(GxEPD_WHITE);

  // Draw grid
  display.drawFastVLine(100, 0, 100, GxEPD_BLACK);
  display.drawFastHLine(0, 100, 200, GxEPD_BLACK);
  display.drawFastHLine(0, 130, 200, GxEPD_BLACK);

  // Draw avatar
  display.drawXBitmap(0, 0, avatar, 100, 100, GxEPD_BLACK);

  // Draw battery status
  if (battery == 100) {
    display.drawXBitmap(180, 0, battery_charging, 24, 24, GxEPD_BLACK);
  } else if (battery == 10) {
     display.drawXBitmap(180, 0, battery_full, 24, 24, GxEPD_BLACK);
  } else if (battery == 7) {
     display.drawXBitmap(180, 0, battery_75, 24, 24, GxEPD_BLACK);
  } else if (battery == 5) {
     display.drawXBitmap(180, 0, battery_50, 24, 24, GxEPD_BLACK);
  } else if (battery == 2) {
     display.drawXBitmap(180, 0, battery_25, 24, 24, GxEPD_BLACK);
  } else if (battery == 0) {
     display.drawXBitmap(180, 0, battery_empty, 24, 24, GxEPD_BLACK);
  }
  
  
  // Draw status
  //display.setFont(&FreeSansBold12pt7b);
  //display.setCursor(105, 42);
  //display.println("H/O");
  //display.drawXBitmap(150, 5, homeoffice, 48, 38, GxEPD_RED);

  // Draw ticket
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(20, 124);
  display.println(ticket);

  // Draw message
  display.drawXBitmap(5, 135, envelope, 26, 20, GxEPD_BLACK);
  display.setCursor(35, 148);
  display.setFont(&FreeSans9pt7b);
  display.println("Great info about anything");

  // Print buffer
  display.update();
}


void drawLowBatteryWindow() {
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_RED);
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(3, 70);
  display.println("  LOW  ");
  display.setCursor(3, 110);
  display.println("BATTERY");
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(22, 140);
  display.println("Connect to charger");
  display.update();
}


bool charging() {
  if (digitalRead(chargePin) == HIGH) {
    return true;
  }
  else {
    return false;
  }
}

void checkBattery() {
  unsigned int raw = 0;
  float volt = 0.0;
  float adjustment = 0.2;
  raw = analogRead(battPin);
  volt = raw / 1023.0;
  volt = volt * 4.2 + adjustment;
  String volts = " volts";
  String v = String(volt + volts);
  Serial.println(v);

  if (volt >= 4.2) {
    battery = 10;
  } else if (volt > 4.1 && volt < 3.9) {
    battery = 7;
  } else if (volt > 3.9 && volt < 3.8) {
    battery = 5;
  } else if (volt > 3.8 && volt < 3.7) {
    battery = 2;
  } else if (volt < 3.7) {
    battery = 0;
  }

  // If volt is too low, update display and go back to sleep for 60 minutes
  float lowVoltCutoff = 3.5;
  if (volt <= lowVoltCutoff) {
    Serial.println("Voltage low, going back to sleep");
    drawLowBatteryWindow();
    ESP.deepSleep(3600 * 1000000);
    Serial.println("Sleep failed!");
  }
  
}


void drawQrCode(String header1, String header2, String text) {
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(5)];
  qrcode_initText(&qrcode, qrcodeData, 5, ECC_HIGH, text.c_str());

  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(3, 18);
  display.println(header1.c_str());
  display.setCursor(3, 32);
  display.println(header2.c_str());


  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(x * 4 + 26, y * 4 + 47, 4, 4, GxEPD_BLACK);
      }
    }
  }
  display.drawXBitmap(50, 100, wifi_inv, 100, 48, GxEPD_WHITE);
  display.drawXBitmap(50, 100, wifi, 100, 48, GxEPD_RED);

  display.update();
  delay(5000);
}

String ssid;
String pw;
String qr_code;

void configModeCallback (WiFiManager *myWiFiManager) {
  drawQrCode("SSID: " + ssid, "Password: " + pw, qr_code);
}

char aws_key[255];
char aws_secret[255];

bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup_aws() {
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(aws_key, json["aws_key"]);
          strcpy(aws_secret, json["aws_secret"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}

void setup() {
  // Start serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setup");

  // Pin modes
  pinMode(chargePin, INPUT);
  pinMode(battPin, INPUT);
  pinMode(D0, WAKEUP_PULLUP);

  // Start display
  display.init();

  // variables
  const int sleepSeconds = 60; // 1800 = 30 minutes
  randomSeed(RANDOM_REG32);
  long randNumber = random(100, 999);
  Serial.print("random2=");
  Serial.print(randNumber);
  ssid = String("Digitar") + String(randNumber);
  long password = random(10000000, 99999999);
  pw = String(password);
  qr_code = String("WIFI:S:") + ssid + String(";T:WPA;P:") + pw + String(";;");

  // Check for low battery and charging status
  if (charging() == true) {
    Serial.println("Charging");
    battery = 100;
  } else {
    Serial.println("Not charging");
    checkBattery();
  }

  // Setup AWS
  setup_aws();

  // Setup AWS params in wifimanager
  WiFiManagerParameter custom_aws_key("key", "AWS Key", aws_key, 255);
  WiFiManagerParameter custom_aws_secret("secret", "AWS Secret", aws_secret, 255);

  // Setup wifimanager
  WiFiManager wifiManager;
  // TODO: allow WiFi reset somehow.
  // wifiManager.resetSettings();
  wifiManager.addParameter(&custom_aws_key);
  wifiManager.addParameter(&custom_aws_secret);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect(ssid.c_str(), pw.c_str());

  strcpy(aws_key, custom_aws_key.getValue());
  strcpy(aws_secret, custom_aws_secret.getValue());

  // Save entered config
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["aws_key"] = aws_key;
    json["aws_secret"] = aws_secret;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  //fetch();
  Serial.println("Setup done");

  // Get data and update, sleep for 30 minutes
  String grumpyTicket = "DEVOPS-123";
  drawStatusWindow(grumpy, grumpyTicket, battery);
  ESP.deepSleep(1800 * 1000000);
}



String fetch() {
  String headers = "";
  String body = "";
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool gotResponse = false;
  long now;

  char host[] = "xxxxxxxx.execute-api.eu-west-1.amazonaws.com"; // Replace xxxx with correct end-point

  if (client.connect(host, 443))
  {
    Serial.println("connected");

    String URL = "/prod/digitar/hellopub";

    Serial.println(URL);

    client.println("GET " + URL + " HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("User-Agent: arduino/1.0");
    client.println("Content-Type: application/json");

    client.println("");

    now = millis();

    while (!client.available()) {
      delay(100);
    }

    Serial.println("Client connected");

    while (client.available())
    {

      char c = client.read();

          Serial.println(String("Char: ") + c);


      if (finishedHeaders)
      {
        body = body + c;
      }
      else
      {
        if (currentLineIsBlank && c == '\n')
        {
          finishedHeaders = true;
        }
        else
        {
          headers = headers + c;
        }
      }

      if (c == '\n')
      {
        currentLineIsBlank = true;
      }
      else if (c != '\r')
      {
        currentLineIsBlank = false;
      }

      gotResponse = true;
    }
    if (gotResponse) {
      Serial.println("Body: " + body);
      return body;
    }
  }

  Serial.println("No HTTP resp");

  return body;
}

void loop() {
}

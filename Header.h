#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>                      
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h> 
#include "time.h"

#define WaterSensor 36
#define WaterPump   33
#define LED_BLUE    15
#define LED_RED     4        
#define Button      5

String ID = "cobacobacobacoba";

LiquidCrystal_I2C lcd(0x27,16,2);
WiFiManager wm;

// API to Application
String apidev = "http://plus-api.herokuapp.com/api/device/r/register";
String apidata = "http://plus-api.herokuapp.com/api/device/" + ID;
String apiget = "http://plus-api.herokuapp.com/api/device/state/" + ID + "/";

// Get Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 21600;
const int   daylightOffset_sec = 3600;

// Milis
unsigned long currentMillis;
unsigned long previousMillisSend = 0;
unsigned long previousMillisLCD = 0;
unsigned long previousMillisGET = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 3000;

// Global Variable
int SensorValue = 0;
int WaterLevel = 0;
int PumpON = 0;
char secTime[10];
int count = 0;
int buttonState;
int lastButtonState = HIGH;

// Fungsi Global
void SetID(String ID);
void GetData();
void SendData(float level);
void ReadSensor(int pin);
void LCDScreen(float level, int condition);
void printLocalTime();

// ===================================================
// Logs GPS (including altitude)and CO2 concentration
// reading data to an SD card.
// Advisor: Brother Todd Lines
// Written by Noah Jorgensen, Daniel Fuller, Kylee 
// Thomas, Brynn Andersen, and Hannah Mills
// For Arduino Mega2560
// ===================================================
#include <SPI.h>
#include <SD.h>
// by Adafruit. Must be manually installed
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
// GPS STUFF
//TX is 7, RX is 6
#define GPSSerial Serial2
Adafruit_GPS GPS(&GPSSerial);
//#define GPSECHO true

// GAS STUFF
//SoftwareSerial gasSerial(8,9); //RX, TX digital pins
unsigned char getGasDensity[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; // read command
//We have two types of dataloggers. If the battery
//    holder is black use pin 9.  If the battery holder
//    is brown then use pin 10.
//File logfile;
char filename[24];
// Variables to read data go here:----
//------------------------------------
RTC_DS1307 rtc;
//float data;
int i = 0;
int N = 50; // Number of samples per file
//int waittime_ms = 500; // milliseconds between samples
// ====================================================
void setup()
{
  Serial.begin(9600);
  init_SD();
  init_RTC();
  // (note 24-hour time: 3pm -> 15)  
  // This line sets the RTC with an
  // explicit date & time, for example:
  // to set January 21, 2014 at 3:18pm
  // you would use the following line:
  //rtc.adjust(DateTime(2023, 5, 31, 16, 9, 0));
    
  //open_next_logfile();
  // GPS STUFF
  //Serial.println("Adafruit GPS library basic parsing test!");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
 
  // GAS STUFF
  // Open serial communications
  //while (!Serial){}
  Serial1.begin(9600);

  //Setup DataFile
  File dataFile = SD.open("CO2DATA.TXT", FILE_WRITE);
  dataFile.println("START OF NEW COLLECT");
  Serial.println("START OF NEW COLLECT");
  dataFile.close();
}
// ==================================================
int timer = millis();
void loop ()
{
    if (millis() - timer > 500)
    {
      timer = millis();
     File logfile = SD.open("CO2DATA.TXT", FILE_WRITE);
     String fullData = String("Day " + String(rtc.now().day()) + "- " + rtc.now().hour()) + ":" + String(rtc.now().minute()) + ":" +
          String(rtc.now().second()) + "," + String(CO2Read()) + ", " + String(GPS.latitude) +" N,"+ String(GPS.longitude) +"W, altitude:"+ String(GPS.altitude);
     logfile.println(fullData);
     Serial.println(fullData);
     logfile.close();
    i++;
    }
  // Reached N samples, open the next log file
  else {
    // change log file name here, if desired.
    i = 0;
  }
}

// =========================================
// initializes the RTC,
// and checks to see if it has been set
// =========================================
void init_RTC()
{
  Serial.print("Initializing RTC...");
  if (!rtc.begin())
  {
    Serial.println(" failed!");
    while (1);
  }
  //Serial.println(" done!");
  if (!rtc.isrunning())
  {
    Serial.println(
    "WARNING: RTC has not been previously set");
  }
}
// ======================================================
// attempts to initialize the SD card for reading/writing
// ======================================================
void init_SD()
{
  Serial.print("Initialize SD...");
  if (!SD.begin(53))
  {
    Serial.println("Fail");
    while (1);
  }
  Serial.println("Done");
}

// ======================================================
// Reads data from the CO2 sensor (SEN220)
// ======================================================
float CO2Read() {
  Serial1.write(getGasDensity,9);
  for(int i = 0; i <= 8; i++) {
    if (Serial1.available() > 0) {
      float hi, lo, CO2;
      int ch = Serial1.read();
      if (i == 2) {
        hi = ch;
      }
      if (i == 3) {
        lo = ch;
      }      
      if (i == 8) {
        // calculate CO2 concentration
        CO2 = hi * 256 + lo;
        // print concentration
        return CO2;
      }
    }
  }
}

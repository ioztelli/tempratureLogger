//For bluetooth
#include <SoftwareSerial.h>
//For RTC
#include <Wire.h>
#include <RTClib.h>
//For Temprature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
//For SD Card Shield
#include <SD.h>
#include <SPI.h>

//Digital 7 for Temprature
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//RTC
RTC_DS1307 RTC;
//Bluetooth ports
SoftwareSerial bt(2, 3);
//SD card
const int chipSelect = 4;
File dataFile;
byte lastSec = 0;

void setup () {
  sensors.begin();
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
  bt.begin(9600);
  if (! RTC.isrunning())
    Serial.println("RTC is NOT running!");

  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1) ;
  }
  Serial.println("card initialized.");

  dataFile = SD.open("templog.csv", FILE_WRITE);
  if (! dataFile) {
    Serial.println("error opening templog.csv");
    // Wait forever since we cant write data
    while (1) ;
  }
}



void loop ()
{
  float tempC;
  tempC = GetTemprature();
  Serial.println(tempC);

  String timeString;
  timeString = TimeNowString();

  DateTime now = RTC.now();
  if (lastSec != now.second())
    Serial.println(timeString);

  bt.println(timeString);
  bt.println(tempC);

  SaveSdCard(timeString, tempC);
  //Records for 1 minute.
  delay(60000);

}
//Get time from RTC and convert to string.
String TimeNowString()
{
  DateTime now = RTC.now();
  String yearString;
  String monthString;
  String dayString;
  String hourString;
  String minuteString;
  String secondString;
  String result;

  yearString = now.year();
  monthString = now.month();
  dayString = now.day();
  hourString = now.hour();
  minuteString = now.minute();
  secondString = now.second();

  result = dayString + "/" + monthString + "/" + yearString + " " + hourString + ":" + minuteString + ":" + secondString;

  return result;
}
//Get temprature value from sensor
float GetTemprature()
{
  float tempC;
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  return tempC;
}

//Save data to csv file
void SaveSdCard(String time, float temprature) {
  String line = time + ";" + String(temprature);
  dataFile.println(line);
  dataFile.flush();
}

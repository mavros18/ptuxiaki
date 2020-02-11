#include <avr/pgmspace.h>
#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager
#include <Streaming.h>
#include <ArduinoJson.h>
#include "I2C_Anything.h"

#define ARDUINO_ID "AR01"
#define SLAVE_ADDRESS 0x40

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte STAT_BLUE = 7;
const byte STAT_GREEN = 8;

const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
//const byte LM35 = A0;


//Constant Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const int min_interval = 1000; //minimun time interval between sensor readings
const char magicCH = '1';
const uint16_t jsonLen = 1000;
//const String sensorName[10] = {"gps", "temperature", "pressure", "humidity", "light", "battery", "sensor07", "sensor08", "sensor09", "sensor10"};

const char sensorName01[] PROGMEM = "gps[o]";			//-|
const char sensorName02[] PROGMEM = "temperature[C]";	// |
const char sensorName03[] PROGMEM = "pressure[Pa]";		// |
const char sensorName04[] PROGMEM = "humidity[%]";		// |
const char sensorName05[] PROGMEM = "light[V]";			// | This table contains the sensor names
const char sensorName06[] PROGMEM = "battery[V]";		// |
const char sensorName07[] PROGMEM = "sensor07";			// |
const char sensorName08[] PROGMEM = "sensor08";			// |
const char sensorName09[] PROGMEM = "sensor09";			// |
const char sensorName10[] PROGMEM = "sensor10";			//-|

const char* const sensorNamePointers[] PROGMEM = {sensorName01, sensorName02, sensorName03, sensorName04, sensorName05, sensorName06, sensorName07, sensorName08, sensorName09, sensorName10}; // The sensorNames pointers (where at in memory)

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
uint8_t number = 0;
float sensorValue[10];	// temp table holding sensors values
char json[jsonLen];		// char table containing the json. max length defined by jsonLen int
char senNameBuffer[14]; // max bytes of sensorName value plus one for "\0"

boolean start_of_request = true; // if this is the beginning of request event
int _sensor_num_ = 0; // counter

void setup() {
  Serial.begin(9600); // init serial communication
  while (!Serial); // wait for serial com to init
  Serial.println(F("Debug: Serial.begin"));

  Wire.begin(SLAVE_ADDRESS); // init I2C communication with device on SLAVE_ADDRESS
  Wire.onReceive(receiveData);	// define onReceive event
  Wire.onRequest(requestData);	// define onRequest event
  Serial.println(F("Debug: Wire.begin"));

  // Pin initialization
  pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
  pinMode(STAT_GREEN, OUTPUT); //Status LED Green

  pinMode(REFERENCE_3V3, INPUT); // define 3v reference
  pinMode(LIGHT, INPUT); // set LIGHT pin as input
  //  pinMode(LM35, INPUT); // if LM35 is used as temperature sensor
  Serial.println(F("Debug: Pin initialization"));

  myPressure.begin(); // Get pressure sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags
  Serial.println(F("Debug: myPressure.begin"));
  
  myHumidity.begin(); // Configure the humidity sensor
  Serial.println(F("Debug: myHumidity.begin"));
  
  inputString.reserve(1); // reserve 200 bytes for the inputString
  
  // init sensorValue table with zeros
  for (int sen = 0; sen < sizeof(sensorValue) / sizeof(float) - 1; sen++) {
    sensorValue[sen] = 0;
  }

  lastSecond = millis(); // track time

  Serial << F("Weather Shield Example\n----------------------") << endl;
}

void loop() {
  //  Serial.println(F("Debug: loop"));
  if (start_of_request) {
    get_sensor_data();
  }
  delay(1000);
  if (stringComplete) {
    stringComplete = false;
    if (inputString == String(magicCH)) {
      if ((millis() - lastSecond) >= min_interval) {
        lastSecond = millis(); // += min_interval;
        get_sensor_data();
      }
      else {
        //Serial.println(F("Too soon!"));
      }
    }
    else {
      Serial.print(millis());
      Serial.print(F(": ERROR: Unknown command ["));
      Serial.print(inputString);
      Serial.print(F("]. Type \""));
      Serial.print(magicCH);
      Serial.println(F("\" to get sensor data."));
    }
    inputString = F(""); // clear the string
  }
}

void serialEvent() {
  Serial.println(F("Debug: serialEvent"));
  while (Serial.available()) {
    char inChar = (char)Serial.read(); // get the new byte
    if (inChar != '\n') {
      inputString += inChar; // add it to the inputString
    } else {
      inputString.trim();
      stringComplete = true;
    }
  }
}

void receiveData(int byteCount) {
  //Serial.println(F("D2ebug: receiveEvent"));

  //  if (howMany >= (sizeof fnum) + (sizeof foo))
  //  {
  //    I2C_readAnything (fnum);
  //    I2C_readAnything (foo);
  //    haveData = true;
  //  }  // end if have enough data

  //  while (Wire.available()) {
  //    number = Wire.read();
  //    inputString = (String)number;
  //    inputString.trim();
  //    stringComplete = true;
  //
  //    Serial.print(F("ARDIe: Hey! I received: "));
  //    Serial.println(inputString);

  String fullsms;
  int i = 0;
  int firstByte = 0;
  int secondByte = 0;
  int bite = 0;
  while (Wire.available()) {
    bite = Wire.read();
    if (i == 0) {
      firstByte = bite;
    }

    if (i == 1) {
      secondByte = bite;
    }

    if (i > 1) {
      fullsms += char(bite);
    }
    i++;
  }
  //  Serial.println(firstByte);
  //  Serial.println(secondByte);
  
  if (fullsms == "RST") {
    Serial.println("ARDIe: Hey! I received: " + fullsms);
    start_of_request = true;
    _sensor_num_ = 0;
  }
}



void requestData() {
  Serial.println("Debug: requestEvent " + String(_sensor_num_));

  if (start_of_request) {
    int resNumber = sizeof(sensorValue) / sizeof(float);
    Wire.write(resNumber);
    start_of_request = false;
  }
  else {
    strcpy_P(senNameBuffer, (char*)pgm_read_word(&(sensorNamePointers[_sensor_num_])));
    String senNameBuf = String(senNameBuffer);
    
    float resNumber = sensorValue[_sensor_num_];
    String t = senNameBuf + "=" + String(resNumber);
    char tt[t.length() + 1];
    t.toCharArray(tt, t.length() + 1);
    Wire.write(tt);
    _sensor_num_++;

    if (_sensor_num_ == (sizeof(sensorValue) / sizeof(float))) {
      start_of_request = true;
      _sensor_num_ = 0;
    }
  }

  //  Wire.write("012345678");
  //  Wire.write("012345678");
  //  Wire.write("012345678");
  //  Wire.write("987654321");
  /*
    for (int sen = 0; sen <= sizeof(sensorValue) / sizeof(float) - 1; sen++) {
      strcpy_P(senNameBuffer, (char*)pgm_read_word(&(sensorNamePointers[sen])));
      String senNameBuf = String(senNameBuffer);

      float resNumber = sensorValue[sen]; //GetTemp();
      String t = String(resNumber) + "~";
      char tt[t.length() + 1];
      t.toCharArray(tt, t.length() + 1);
      Wire.write(tt);
      Serial.println(tt);
    }
  */
  //  get_sensor_data();
  //  Serial.println(resNumber);

}

//double GetTemp(void)
//{
//  unsigned int wADC;
//  double t;
//  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
//  ADCSRA |= _BV(ADEN); // enable the ADC
//  delay(20); // wait for voltages to become stable.
//  ADCSRA |= _BV(ADSC); // Start the ADC
//  while (bit_is_set(ADCSRA, ADSC));
//  wADC = ADCW;
//  t = (wADC - 324.31 ) / 1.22;
//  return (t);
//}



//
//void sendData(char* senName, float senValue) {
//  //    Wire.beginTransmission();
//  Serial << millis() << F(": Sending data... [") << senName << F("=") << senValue;
//  I2C_writeAnything (senName);
//  I2C_writeAnything (senValue);
//  //  I2C_writeAnything (sensorValue[sen]);
//  //  I2C_writeAnything ("~");
//  //  Wire.endTransmission();
//
//  //  Serial << millis() << F(": Data sent: ") << senNameBuffer << sensorValue[sen] << endl;
//  delay(1000);
//}

float get_battery_level() {
  //  Returns the voltage of the raw pin based on the 3.3V rail
  //  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
  //  Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
  //  3.9K on the high side (R1), and 1K on the low side (R2)
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float rawVoltage = analogRead(BATT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  return (rawVoltage);
}

float get_light_level() {
  //  Returns the voltage of the light sensor based on the 3.3V rail
  //  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float lightSensor = analogRead(LIGHT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor = operatingVoltage * lightSensor;
  return (lightSensor);
}

void get_sensor_data() {

  digitalWrite(STAT_BLUE, HIGH);
  float humidity = myHumidity.readHumidity(); //Check Humidity Sensor
  if (humidity == 998) { //Humidty sensor failed to respond
    Serial.println(F("I2C communication to sensors is not working. Check solder connections."));
    myPressure.begin();               //-|
    myPressure.setModeBarometer();    // |
    myPressure.setOversampleRate(7);  // | Try re-initializing the I2C comm and the sensors
    myPressure.enableEventFlags();    // |
    myHumidity.begin();               //-|
  }
  else {

    float humidity_h = humidity;
    float temp_h = myHumidity.readTemperature();
    float pressure = myPressure.readPressure();

    //    float tempf = myPressure.readTempF();
    float light_lvl = get_light_level();
    float batt_lvl = get_battery_level();




    //    float reading_lm35 = analogRead(pgm_read_byte(LM35));
    //    float temp_lm35 = (reading_lm35 * 500) / 1023; //Celcius
    //    float temp_lm35_f = (temp_lm35 * 1.8) + 32; //Fahrenheit

    //    Serial.print(ARDUINO_ID);
    //    Serial.print(F(" "));
    //    Serial.print(millis());
    //    Serial.print(F(":\t"));

    for (int sen = 0; sen <= sizeof(sensorValue) / sizeof(float) - 1; sen++) {
      strcpy_P(senNameBuffer, (char*)pgm_read_word(&(sensorNamePointers[sen])));
      String senNameBuf = String(senNameBuffer);
      if (senNameBuf == "gps[o]") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuf == "temperature[C]") {
        sensorValue[sen] = temp_h;
      }
      else if (senNameBuf == "pressure[Pa]") {
        sensorValue[sen] = pressure;
      }
      else if (senNameBuf == "humidity[%]") {
        sensorValue[sen] = humidity_h;
      }
      else if (senNameBuf == "light[V]") {
        sensorValue[sen] = light_lvl;
      }
      else if (senNameBuf == "battery[V]") {
        sensorValue[sen] = batt_lvl;
      }
      else if (senNameBuf == "sensor07") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuf == "sensor08") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuf == "sensor09") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuf == "sensor10") {
        sensorValue[sen] = 0;
      }
      else {
        Serial.print("Error: Not found in sensors list:");
        Serial.println(senNameBuf);
        sensorValue[sen] = 0;
        continue;
      }

      //      sendData(senNameBuffer,sensorValue[sen]);
      //      Serial << senNameBuffer << F("=") << sensorValue[sen] << F(" ");
    }
    //    Serial.print(F("\n"));
  }
  digitalWrite(STAT_BLUE, LOW);
}
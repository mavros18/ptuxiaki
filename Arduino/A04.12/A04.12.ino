#include <avr/pgmspace.h>
#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager
#include <Streaming.h>
#include <ArduinoJson.h>

#define ARDUINO_ID "0001"
#define SLAVE_ADDRESS 0x40

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte PROGMEM STAT_BLUE = 7;
const byte PROGMEM STAT_GREEN = 8;

const byte PROGMEM REFERENCE_3V3 = A3;
const byte PROGMEM LIGHT = A1;
const byte PROGMEM BATT = A2;
const byte PROGMEM LM35 = A0;


//Constant Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const uint8_t min_interval PROGMEM = 1000; //minimun time interval between sensor readings
const char magicCH PROGMEM  = '1';
const uint8_t jsonLen = 400;
//const String s/ensorName[10] = {"gps", "temperature", "pressure", "humidity", "light", "battery", "sensor07", "sensor08", "sensor09", "sensor10"};

const char sensorName01[] PROGMEM = "gps";
const char sensorName02[] PROGMEM = "temperature";
const char sensorName03[] PROGMEM = "pressure";
const char sensorName04[] PROGMEM = "humidity";
const char sensorName05[] PROGMEM = "light";
const char sensorName06[] PROGMEM = "battery";
const char sensorName07[] PROGMEM = "sensor07";
const char sensorName08[] PROGMEM = "sensor08";
const char sensorName09[] PROGMEM = "sensor09";
const char sensorName10[] PROGMEM = "sensor10";

const char* const sensorNamePointers[] PROGMEM = {sensorName01, sensorName02, sensorName03, sensorName04, sensorName05, sensorName06, sensorName07, sensorName08, sensorName09, sensorName10};




//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
uint8_t number = 0;
float sensorValue[10];
char json[jsonLen];
char senNameBuffer[12]; //max bytes of sensorName value plus one for "\0"


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // Pin initialization
  pinMode(pgm_read_byte(STAT_BLUE), OUTPUT); //Status LED Blue
  pinMode(pgm_read_byte(STAT_GREEN), OUTPUT); //Status LED Green

  pinMode(pgm_read_byte(REFERENCE_3V3), INPUT);
  pinMode(pgm_read_byte(LIGHT), INPUT);
  pinMode(pgm_read_byte(LM35), INPUT);

  // Configure LM35
  //~ analogReference(DEFAULT);

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();
  inputString.reserve(200); // reserve 200 bytes for the inputString

  for (int sen = 0; sen < sizeof(sensorValue) / sizeof(float) - 1; sen++) {
    sensorValue[sen] = 0;
    //    sensorName[sen]="";
  }

  lastSecond = millis();

  Serial << "Weather Shield Example" << "\n----------------------" << endl;

}

void loop() {
  //  stringComplete = true;
  //  inputString = "get";
  //  delay(1000);

  if (stringComplete) {
    stringComplete = false;
    if (inputString == String(pgm_read_word(magicCH))) {
      if (millis() - lastSecond >= pgm_read_word(min_interval)) {
        lastSecond = millis(); // += min_interval;
        get_sensor_data();
        //        print_sensor_data();

        createJson();
        Serial.println(json);
      }
      //      else{
      //          Serial.println("Too soon!");
      //      }
    }
    else {
      Serial.print(millis());
      Serial.print(F(": ERROR: Unknown command ["));
      Serial.print(inputString);
      Serial.print(F("]. Type \""));
      Serial.print(pgm_read_word(magicCH));
      Serial.println(F("\" to get sensor data."));
    }
    inputString = ""; // clear the string

  }

  //  Serial.println();
  //  Serial.println("--- Parsed ---");
  //  readJson();

}

void serialEvent() {
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
  while (Wire.available()) {
    number = Wire.read();
    Serial.print(F("ARD:Hey Raspberry, I received: "));
    Serial.println(number);

    //if (number != '\n'){
    //  inputString += number; // add it to the inputString
    //}else{
    inputString = (String)number;
    inputString.trim();
    stringComplete = true;
    //}
  }
}

void sendData() {
  //Wire.write(number);
  Wire.write("elinikos");
  Serial.print(" data sent: ");
  Serial.println(number);
  delay(1000);
}

float get_battery_level() {
  //  Returns the voltage of the raw pin based on the 3.3V rail
  //  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
  //  Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
  //  3.9K on the high side (R1), and 1K on the low side (R2)
  float operatingVoltage = analogRead(pgm_read_byte(REFERENCE_3V3));
  float rawVoltage = analogRead(pgm_read_byte(BATT));
  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  return (rawVoltage);
}

float get_light_level() {
  //  Returns the voltage of the light sensor based on the 3.3V rail
  //  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
  float operatingVoltage = analogRead(pgm_read_byte(REFERENCE_3V3));
  float lightSensor = analogRead(pgm_read_byte(LIGHT));
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor = operatingVoltage * lightSensor;
  return (lightSensor);
}

void get_sensor_data() {
  digitalWrite(pgm_read_byte(STAT_BLUE), HIGH);
  float humidity = myHumidity.readHumidity(); //Check Humidity Sensor
  if (humidity == 998) { //Humidty sensor failed to respond
    Serial.println(F("I2C communication to sensors is not working. Check solder connections."));
    myPressure.begin();               //-|
    myPressure.setModeBarometer();    // |
    myPressure.setOversampleRate(7);  // | Try re-initializing the I2C comm and the sensors
    myPressure.enableEventFlags();    // |
    myHumidity.begin();               //-|
  } else {
    float humidity_h = humidity;
    //    float temp_h = myHumidity.readTemperature();
    //    float pressure = myPressure.readPressure();

    //    float tempf = myPressure.readTempF();
    //    float light_lvl = get_light_level();
    //    float batt_lvl = get_battery_level();

    //    float reading_lm35 = analogRead(pgm_read_byte(LM35));
    //    float temp_lm35 = (reading_lm35 * 500) / 1023; //Celcius
    //    float temp_lm35_f = (temp_lm35 * 1.8) + 32; //Fahrenheit


    for (int sen = 0; sen < sizeof(sensorValue) / sizeof(float) - 1; sen++) {

      strcpy_P(senNameBuffer, (char*)pgm_read_word(&(sensorNamePointers[sen])));


      //      Serial.print(sensorName[sen]);
      if (senNameBuffer == "gps") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuffer == "temperature") {
        sensorValue[sen] = myHumidity.readTemperature();//temp_h;
      }
      else if (senNameBuffer == "pressure") {
        sensorValue[sen] = myPressure.readPressure();//pressure;
      }
      else if (senNameBuffer == "humidity") {
        sensorValue[sen] = humidity_h;
      }
      else if (senNameBuffer == "light") {
        sensorValue[sen] = get_light_level();//light_lvl;
      }
      else if (senNameBuffer == "battery") {
        sensorValue[sen] = get_battery_level();//batt_lvl;
      }
      else if (senNameBuffer == "sensor07") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuffer == "sensor08") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuffer == "sensor09") {
        sensorValue[sen] = 0;
      }
      else if (senNameBuffer == "sensor10") {
        sensorValue[sen] = 0;
      }
      else {
        sensorValue[sen] = 0;
      }

      Serial << senNameBuffer << "=" << sensorValue[sen] << " ";
      //    sensorName[sen]="";
    }
    Serial.print("\n");
    //    sensorValue[0] = humidity_h;
    //    sensorValue[1] = temp_h;
    //    sensorValue[2] = pressure;
    //    sensorValue[3] = tempf;
    //    sensorValue[4] = light_lvl;
    //    sensorValue[5] = batt_lvl;


    //    sensor[6] = temp_lm35;
  }
  digitalWrite(pgm_read_byte(STAT_BLUE), LOW);
}

void print_sensor_data() {
  digitalWrite(pgm_read_byte(STAT_GREEN), HIGH);

  Serial.print(millis());
  Serial.print(": ");

  //  for (int sen = 0; sen < sizeof(sensorValue) / sizeof(float); sen++) {
  //    Serial << sensorName[sen] << "=" << sensorValue[sen] << " ";
  //  }
  Serial.println();

  //  Serial.print("Humidity = ");
  //  Serial.print(sensorValue[0]);
  //  Serial.print("%,");
  //  Serial.print(" temp_h = ");
  //  Serial.print(sensorValue[1],2);
  //  Serial.print("C,");
  //
  //  //Check Pressure Sensor
  //  Serial.print(" Pressure = ");
  //  Serial.print(sensorValue[2]);
  //  Serial.print("Pa,");
  //
  //  //Check tempf from pressure sensor
  //  Serial.print(" temp_p = ");
  //  Serial.print(sensorValue[3], 2);
  //  Serial.print("F,");
  //
  //  //Check light sensor
  //  Serial.print(" light_lvl = ");
  //  Serial.print(sensorValue[4]);
  //  Serial.print("V,");
  //
  //  //Check batt level
  //  Serial.print(" VinPin = ");
  //  Serial.print(sensorValue[5]);
  //  Serial.print("V,");

  //LM35
  //  Serial.print(" Temp_LM35 = ");
  //  Serial.print(sensor[6]);
  //  Serial.print("C");

  digitalWrite(pgm_read_byte(STAT_GREEN), LOW);
}

void createJson() {
  //
  // Reserve memory space
  //
  StaticJsonBuffer<jsonLen> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["ARDID"] = ARDUINO_ID;

  JsonArray& sensors_array = root.createNestedArray("sensors");

  for (int sen = 0; sen < sizeof(sensorValue) / sizeof(float) - 1; sen++) {
    JsonObject& sensor = sensors_array.createNestedObject();
    //    sensor["name"] = sensorName[sen];
    sensor["time"] = millis();
    JsonArray& data_array = sensor.createNestedArray("data");
    data_array.add(sensorValue[sen], 6);
  }

  //
  // Step 3: Generate the JSON string
  //
  int len = root.measureLength();
  Serial << F("OutLen: ") << len << endl;
  root.prettyPrintTo(json);
}

//void readJson(){
//  //
//  // Reserve memory space
//  //
//  StaticJsonBuffer<jsonLen> jsonBuffer;
//
//  //
//  // Deserialize the JSON string
//  //
//  JsonObject& root = jsonBuffer.parseObject(json);
//
//  if (!root.success())
//  {
//    Serial.println(F("parseObject() failed"));
//    return;
//  }
//
//  //
//  // Retrieve the values
//  //
//  const char* id = root["ARDID"];
//  const char* sensor = root["sensors"]["name"];
//  long        timeJ = root["sensors"]["time"];
//  double      lat = root["sensors"]["data"][0];
//  double      lon = root["sensors"]["data"][1];
//
//  Serial << F("ARDID: ") << id << endl;
////  Serial.println(sensor);
//  Serial << F("Sensor: ") << sensor << endl;
////  Serial.println(timeJ);
//  Serial << F("Time: ") << timeJ << endl;
////  Serial.println(latitude);
//  Serial << F("Lat: ") <<  _FLOAT(lat,6) << endl;
////  Serial.println(longitude);
//  Serial << F("Lon: ") << _FLOAT(lon,6) << endl;
//  Serial.println();
//}

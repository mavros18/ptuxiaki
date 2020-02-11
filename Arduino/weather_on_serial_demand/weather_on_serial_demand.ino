/*
 Weather Shield Example
 By: Nathan Seidle
 SparkFun Electronics
 Date: June 10th, 2016
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example prints the current humidity, air pressure, temperature and light levels.

 The weather shield is capable of a lot. Be sure to checkout the other more advanced examples for creating
 your own weather station.

 */

#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager

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
const byte LM35 = A0;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int min_interval = 2000; //minimun time interval between sensor readings
int number=0;

float sensor[7];
int i;
  
void setup(){
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  //Wire.onRequest(sendData);
  Serial.println("Weather Shield Example");
  Serial.println("----------------------");

  pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
  pinMode(STAT_GREEN, OUTPUT); //Status LED Green

  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);
  pinMode(LM35,INPUT);

  // Configure LM35
  // analogReference(DEFAULT);
  
  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();
  inputString.reserve(200); // reserve 200 bytes for the inputString
  
  sensor[0]=0;
  sensor[1]=0;
  sensor[2]=0;
  sensor[3]=0;
  sensor[4]=0;
  sensor[5]=0;
  sensor[6]=0;
  
  lastSecond = millis();
//  Serial.println("Weather Shield online!");
}

void loop(){
  stringComplete = true;
  inputString = "1";
  delay(1000);
  
    if (stringComplete){
      
      if (inputString == "1"){
        if (millis() - lastSecond >= min_interval){
          lastSecond = millis(); //+= min_interval;
          get_sensor_data();
          print_sensor_data();
          Wire.write(1);
        }else{
//          Serial.println("Too soon!");
        }
      }else{
        Serial.print(millis());
        Serial.print(": ERROR: Unknown command [");
        Serial.print(inputString);
        Serial.println("]. Type \"g\" to get sensor data.");
      }
      inputString = ""; // clear the string
      stringComplete = false;
    }
}

void print_sensor_data(){
  digitalWrite(STAT_GREEN, HIGH);
  
  Serial.print(millis());
  Serial.print(": ");
  
  Serial.print("Humidity = ");
  Serial.print(sensor[0]);
  Serial.print("%,");
  Serial.print(" temp_h = ");
  Serial.print(sensor[1],2);
  Serial.print("C,");

  //Check Pressure Sensor
  Serial.print(" Pressure = ");
  Serial.print(sensor[2]);
  Serial.print("Pa,");

  //Check tempf from pressure sensor
  Serial.print(" temp_p = ");
  Serial.print(sensor[3], 2);
  Serial.print("F,");

  //Check light sensor
  Serial.print(" light_lvl = ");
  Serial.print(sensor[4]);
  Serial.print("V,");

  //Check batt level
  Serial.print(" VinPin = ");
  Serial.print(sensor[5]);
  Serial.print("V,");

  //LM35
//  Serial.print(" Temp_LM35 = ");
//  Serial.print(sensor[6]);
//  Serial.print("C");
  
  Serial.println();
  digitalWrite(STAT_GREEN, LOW);
}

void get_sensor_data(){
  digitalWrite(STAT_BLUE, HIGH);
  float humidity = myHumidity.readHumidity(); //Check Humidity Sensor
  if (humidity == 998){ //Humidty sensor failed to respond
    Serial.println("I2C communication to sensors is not working. Check solder connections.");
    myPressure.begin();               //-|
    myPressure.setModeBarometer();    // |
    myPressure.setOversampleRate(7);  // | Try re-initializing the I2C comm and the sensors
    myPressure.enableEventFlags();    // |
    myHumidity.begin();               //-|
  }else{
    float humidity_h = humidity;
    float temp_h = myHumidity.readTemperature();
    float pressure = myPressure.readPressure();
  
    float tempf = myPressure.readTempF();
    float light_lvl = get_light_level();
    float batt_lvl = get_battery_level();
  
//    float reading_lm35 = analogRead(LM35);  
//    float temp_lm35 = (reading_lm35 * 500) / 1023; //Celcius
//    float temp_lm35_f = (temp_lm35 * 1.8) + 32; //Fahrenheit
  
    sensor[0] = humidity_h;
    sensor[1] = temp_h;
    sensor[2] = pressure;
    sensor[3] = tempf;
    sensor[4] = light_lvl;
    sensor[5] = batt_lvl;
//    sensor[6] = temp_lm35;
  }
  digitalWrite(STAT_BLUE, LOW);
}

//  Returns the voltage of the light sensor based on the 3.3V rail
//  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level(){
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float lightSensor = analogRead(LIGHT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor = operatingVoltage * lightSensor;
  return (lightSensor);
}

//  Returns the voltage of the raw pin based on the 3.3V rail
//  This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//  Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//  3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level(){
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float rawVoltage = analogRead(BATT);
  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  return (rawVoltage);
}
void receiveData(int byteCount){
  while(Wire.available()){
    number=Wire.read();
    Serial.print("ARD:Hey Raspberry, I received: ");
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
void serialEvent(){
  while (Serial.available()){
    char inChar = (char)Serial.read(); // get the new byte
    if (inChar != '\n'){
      inputString += inChar; // add it to the inputString
    }else{
      inputString.trim();
      stringComplete = true;
    }
  }
}

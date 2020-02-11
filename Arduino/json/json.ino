#include <Streaming.h>
#include <ArduinoJson.h>

#define ARDUINO_ID "0001"


const int jsonLen = 500;
char json[jsonLen];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("--- Created ---");
  createJson();
  Serial.println(json);
  delay(100);

  Serial.println();
  Serial.println("--- Parsed ---");
  readJson();
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void createJson(){
  //
  // Step 1: Reserve memory space
  //
  StaticJsonBuffer<jsonLen> jsonBuffer;
  
  //
  // Step 2: Build object tree in memory
  // {"sensor":"gps","time":29450,"data":[48.756080,2.302038]}
  //
  // {"ARDID":"0001","sensor":["name":"gps","time":123123,"data":[01.123456,02.123456]}
  JsonObject& root = jsonBuffer.createObject();
  root["ARDID"] = ARDUINO_ID;
  
  //JsonArray& data = root.createNestedArray("sensors");
  //data.add(48.756080, 6);  // 6 is the number of decimals to print
  //data.add(2.302038, 6);   // if not specified, 2 digits are printed
  JsonArray& sensors_array = root.createNestedArray("sensors");
  
  JsonObject& sensor_gps = sensors_array.createNestedObject();
  sensor_gps["name"]="gps";
  sensor_gps["time"]=123123;
  JsonArray& gps_data_array = sensor_gps.createNestedArray("data");
  gps_data_array.add(1.123456, 6);
  gps_data_array.add(2.123456, 6);

  JsonObject& sensor_temp = sensors_array.createNestedObject();
  sensor_temp["name"]="temp";
  sensor_temp["time"]=123123;
  JsonArray& temp_data_array = sensor_temp.createNestedArray("data");
  temp_data_array.add(23.2);

  JsonObject& sensor_pressure = sensors_array.createNestedObject();
  sensor_pressure["name"]="pressure";
  sensor_pressure["time"]=123123;
  JsonArray& pressure_data_array = sensor_pressure.createNestedArray("data");
  pressure_data_array.add(900000);

  JsonObject& sensor_humidity = sensors_array.createNestedObject();
  sensor_humidity["name"]="humidity";
  sensor_humidity["time"]=123123;
  JsonArray& humidity_data_array = sensor_humidity.createNestedArray("data");
  humidity_data_array.add(30);

  
  //
  // Step 3: Generate the JSON string
  //
  int len = root.measureLength();
  Serial << "OutLen: " << len << endl;
  root.prettyPrintTo(json);
}

void readJson(){
  //
  // Step 1: Reserve memory space
  //
  StaticJsonBuffer<jsonLen> jsonBuffer;
  
  //
  // Step 2: Deserialize the JSON string
  //
  JsonObject& root = jsonBuffer.parseObject(json);
  
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
  
  //
  // Step 3: Retrieve the values
  //
  const char* id = root["ARDID"];
  const char* sensor = root["sensors"]["name"];
  long        timeJ = root["sensors"]["time"];
  double      lat = root["sensors"]["data"][0];
  double      lon = root["sensors"]["data"][1];

  Serial << "ARDID: " << id << endl;
//  Serial.println(sensor);
  Serial << "Sensor: " << sensor << endl;
//  Serial.println(timeJ);
  Serial << "Time: " << timeJ << endl;
//  Serial.println(latitude);
  Serial << "Lat: " <<  _FLOAT(lat,6) << endl;
//  Serial.println(longitude);
  Serial << "Lon: " << _FLOAT(lon,6) << endl;
  Serial.println();
}




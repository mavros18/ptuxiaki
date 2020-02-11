#include "I2C_Anything.h"
#include <Wire.h>
#include <Streaming.h>

#define SLAVE_ADDRESS 0x40

//String sms = "";
int bite = 0;
//String fullsms ="";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

}

void loop() {
  //  char sms[] = "hello";
  //
  //  char* buf = sms;

  //  ProcessRequestString();
  //  Serial.println(GetTemp());
  //  Serial.println();
  //  delay(1000);
  ////  I2C_writeAnything (sensor/Name01);
  //  Wire.write(buf);
  //
  //  Serial << millis() << ": " << buf << endl;
//  sendData();
//  delay(1000);

}

void receiveData(int byteCount) {
  Serial.println("\nIncoming...");
  String fullsms;
  int i = 0;
  int firstByte = 0;
  int bytesLen = 0;
  while (Wire.available()) {
    bite = Wire.read();
    if (i == 0) {
      firstByte = bite;
    }

    if (i == 1) {
      bytesLen = bite;
    }

    if (i > 1) {
      fullsms += char(bite);
    }
    i++;
  }
  //  Serial.println(firstByte);
  //  Serial.println(bytesLen);
  Serial.println("RSP: " + fullsms);
  //sendData();
}

void sendData() {
  Serial.println("\nOutgoing...");
  float resNumber = GetTemp();
  String t = String(resNumber);
  char tt[t.length()+1];
  t.toCharArray(tt, t.length()+1);
  Wire.write("012345678");
  Wire.write("012345678");
  Wire.write("012345678");
  Wire.write("987654321");
  Serial.println(resNumber);
  Serial.println(tt);
}

//void ProcessRequestString() {
//  Serial.print(millis());
//  Serial.println(": Sending to wire...");
//  Serial.println(sms.c_str());
//  Wire.write(sms.c_str());
//}

double GetTemp(void)
{
  unsigned int wADC;
  double t;
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN); // enable the ADC
  delay(20); // wait for voltages to become stable.
  ADCSRA |= _BV(ADSC); // Start the ADC
  while (bit_is_set(ADCSRA, ADSC));
  wADC = ADCW;
  t = (wADC - 324.31 ) / 1.22;
  return (t);
}

//void sendData(float senValue) {
//  //    Wire.beginTransmission();
//  Serial << millis() << F(": Sending data...") << senValue << endl;
//  I2C_writeAnything (senValue);
//  //  I2C_writeAnything (sensorValue[sen]);
//  //  I2C_writeAnything ("~");
//  //  Wire.endTransmission();
//
//  //  Serial << millis() << F(": Data sent: ") << senNameBuffer << sensorValue[sen] << endl;
//  delay(1000);
//}

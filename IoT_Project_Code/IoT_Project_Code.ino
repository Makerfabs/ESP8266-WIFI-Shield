#include <SoftwareSerial.h>
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define RXPIN 5
#define TXPIN 6

#define BAUDRATE 9600
#define DHTTYPE DHT11 

SoftwareSerial mySerial(RXPIN, TXPIN);
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(BAUDRATE);
    mySerial.begin(BAUDRATE);
    dht.begin();

    delay(2000);
    Serial.println("Begin");

    sendData("AT", 3000);
    sendData("AT+GMR", 3000);
    sendData("AT+CWMODE=1", 3000);
    sendData("AT+CWJAP=\"Makerfabs\",\"20160704\"", 3000);


}


void loop()
{
  delay(6000);
    String temp = "AT+HTTPCLIENT=2,0,\"http://api.thingspeak.com/update?api_key=1Z06P9A6WFHM00OP&field1=";
    int temperature = dht.readTemperature();
    int humdity = dht.readHumidity();
    temp = temp + temperature + "&field2=" + humdity + "\",,,1";

    sendData(temp,3000);
    // get what form serial send to USB serial
    while (mySerial.available() > 0)
    {
        Serial.write(mySerial.read());
        yield();
    }
    // get what form USB serial send to serial
    while (Serial.available() > 0)
    {
        mySerial.write(Serial.read());
        yield();
    }
}

// AT send
String sendData(String command, const int timeout)
{
    String response = "";
    mySerial.println(command);

    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (mySerial.available())
        {
            String c = mySerial.readString();
            response += c;
        }
    }

    Serial.println(response);
    return response;
}

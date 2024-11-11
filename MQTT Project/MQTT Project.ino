#include <SoftwareSerial.h>
#define RXPIN 5
#define TXPIN 6
#define LEDPIN 3
#define BAUDRATE 9600
#define DHTTYPE DHT11

#define MQTT_TOPIC "/public/TEST/makerfabs-T"

SoftwareSerial mySerial(RXPIN, TXPIN);


void setup()
{
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, HIGH);

    Serial.begin(BAUDRATE);
    mySerial.begin(BAUDRATE);


    delay(2000);
    Serial.println("Begin");

    sendData("AT", 3000);
    sendData("AT+GMR", 3000);
    sendData("AT+CWMODE=1", 3000);
    sendData("AT+CWJAP=\"Makerfabs\",\"20160704\"", 3000);

    delay(10000);

    mqtt_init(MQTT_TOPIC);
}

void loop()
{
    mqtt_listen(MQTT_TOPIC);
}


//MQTT configuration
void mqtt_init(String topic)
{
    String respect = "";

    //Close an existing connection
    respect = sendData("AT+MQTTCLEAN=0", 3000);
    Serial.println(respect);

    //configuration
    respect = sendData("AT+MQTTUSERCFG=0,1,\"mqttx_01b6889a\",\"\",\"\",0,0,\"mqtt://test.mosquitto.org\"", 3000); 
    Serial.println(respect);

    //connection
    respect = sendData("AT+MQTTCONN=0,\"test.mosquitto.org\",1883,1", 3000);
    Serial.println(respect);

    //subscription
    String temp = "AT+MQTTSUB=0,\"";
    temp = temp + topic + "\",0";
    respect = sendData(temp, 3000);
    Serial.println(respect);
}

//MQTT LISTENING
void mqtt_listen(String topic)
{
    while (mySerial.available())
    {
        String temp = mySerial.readString();
        if (temp.indexOf(topic) != -1)
        {
            String result = "";
            result = mqtt_analyse(topic, temp);
            if (result.indexOf("ON") != -1)
            {
                digitalWrite(LEDPIN, LOW);
            }
            if (result.indexOf("OFF") != -1)
            {
                digitalWrite(LEDPIN, HIGH);
            }
        }
        else
        {
            Serial.println(temp);
        }
    }
}

//MQTT Reply to parse
//+MQTTSUBRECV:0,"/public/TEST/makerfabs-T",2,ON
String mqtt_analyse(String topic, String str)
{
    int offset = topic.length();

    String msg = "";
    msg = str.substring(str.indexOf(topic) + offset + 2);   //

    int size = msg.substring(0, msg.indexOf(",") + 1).toInt();
    msg = msg.substring(msg.indexOf(",") + 1);

    Serial.println("Origin:");
    Serial.println(str);
    Serial.println("Topic:");
    Serial.println(topic);
    Serial.println("Length:");
    Serial.println(size);
    Serial.println("Context:");
    Serial.println(msg);

    return msg;
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

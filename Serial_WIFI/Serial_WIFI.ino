/*********************
ESP8266 as a TCP Servicer
*********************/
#include<SoftwareSerial.h>
//Create a new softSerial object，rx:5,tx:6
SoftwareSerial softSerial1(5, 6);
#define DEBUG true
#define SSID "Makerfabs"  // your ssid
#define PASS "20160704" //pass word
void setup()
{
  Serial.begin(9600);
  softSerial1.begin(9600); // your esp's baud rate might be different
 
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point 
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWJAP=\"Makerfabs\",\"20160704\"\r\n",5000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG);// get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}
 
void loop()
{
  if(softSerial1.available()) // check if the esp is sending a message 
  {

    if(softSerial1.find("+IPD,"))
    {
     delay(1000);
 
     int connectionId = softSerial1.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
     String webpage = "<h1>Hello,This is Makerfabs!</h1>";
 
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);

     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=5;
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
     sendData("AT+RESTORE\r\n",3000,DEBUG);
    }
  }
}
 
 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    softSerial1.print(command); // send the read character to the Serial
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(softSerial1.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = softSerial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

boolean connectWiFi()
{
       sendData("AT+CWJAP=\"Makerfabs\",\"20160704\"\r\n",5000,DEBUG);
       delay(5000);
       if(softSerial1.find("OK"))
     {
          Serial.println("OK, Connected to WiFi.");
     return true;
     }else
     {
           Serial.println("Can not connect to the WiFi.");
           return false;
     }
}

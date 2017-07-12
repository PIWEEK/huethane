/*

 This example connects to an unencrypted Wifi network.
 Then it prints the  MAC address of the Wifi shield,
 the IP address obtained, and other network details.

 Circuit:
 * WiFi shield attached

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFi.h>
#include <SoftwareSerial.h>

char ssid[] = "Kaleidos_2.4";     //  your network SSID (name)
char pass[] = "KaleidosNet";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

IPAddress server(10,8,1,153);
//IPAddress server2(10,8,1,186);
const char hueHubIP[] = "10.8.1.153";  // Hue hub IP
const char hueUsername[] = "Ane1QtgVvSjNsxbvacqhyRL748t25fP8vbx5uIYG";  // Hue username
const int hueHubPort = 80;

SoftwareSerial mySerial(3,2); // pin 2 = TX, pin 3 = RX (unused)

//  Hue variables
int color = 0;

WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mySerial.begin(9600); // set up serial port for 9600 baud
  delay(500); // wait for display to boot up

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }
   delay(5000);
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}

int i=0;
int values[60];
int valuethreshold = 40;
void loop() {

  delay(1000);
    
  int sensorValue = analogRead(A0);
  int gasoffset = min(sensorValue/2,255);
  values[i%60] = sensorValue;

  for (int a=0; a<60; a++){
    if (values[a] > valuethreshold){
      color = 0;
      break;
    }
    else {
      color = 25500;
    }
    
  }
  
  String command = "{\"on\":true, \"hue\":";
  command = command + color;
  command = command + ",\"bri\":";
  command = command + gasoffset;
  command = command + "}";

  //sendValue(sensorValue);

  if (i%10==0){
    setHue(1, command);
  }

  Serial.println(sensorValue);
  Serial.println(command);
  printLCD(sensorValue);

  i++;
}

//void sendValue(int sensorValue)
//{
//
//  char url[40];
//  sprintf(url, "GET /?x=%d HTTP/1.1", sensorValue);
//  
//  Serial.print("URL: ");
//  Serial.println(url);
//  client.stop();
//  client.connect(server2,8000); 
//  client.println(url);
//  client.println("Host: 10.8.1.186");                          
//  client.println("Connection: close");
//  client.println();  
//
//  
//}

/* setHue() is our main command function, which needs to be passed a light number and a 
 * properly formatted command string in JSON format (basically a Javascript style array of variables
 * and values. It then makes a simple HTTP PUT request to the Bridge at the IP specified at the start.
 */
boolean setHue(int lightNum,String command)
{
  Serial.println(hueHubIP);
  Serial.println(hueHubPort);
   client.stop();
  client.connect(server, 80);
  
    Serial.println("client.connect");
     delay(1000);
    if (client.connected())
    {
     

      Serial.println("if client.connected");
      //client.println("PUT /api/Ane1QtgVvSjNsxbvacqhyRL748t25fP8vbx5uIYG/lights/1/state HTTP/1.1");
      client.print("PUT /api/");
      client.print(hueUsername);
      client.print("/lights/");
      client.print(lightNum);  // hueLight zero based, add 1
      client.println("/state HTTP/1.1");
      client.println("keep-alive");
      client.print("Host: ");
      client.println(hueHubIP);
      client.print("Content-Length: ");
      client.println(command.length());
   
      client.println("Content-Type: text/plain;charset=UTF-8");
      client.println();  // blank line before body
      client.println(command);  // Hue command
    }
   
   
    return true;  // command executed
  
//  else
//    return false;  // command failed
}


void printLCD(int value)
{
  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);

  mySerial.write("                "); // clear display
  mySerial.write("                ");

  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);

  char message[4];
  sprintf(message,"Methane level: %d", value);
  mySerial.write(message);
  
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

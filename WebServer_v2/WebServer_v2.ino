/*
  Since Arduino can't handle https, we need to use Pushingbox API (uses http) to run the Google Script (uses https).
  This script creates a connection to my google excel file and updates temperature accoring to the ping rate set.

  
*/

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
long log_time_ms = 5000; // how often to log data in milliseconds
long prev_log_time = 0;   // previous time log occurred

//-------------------------------------------------------------------------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Setting MAC Address
char website[] = "api.pushingbox.com"; //pushingbox API server
//char website[] = "www.google.com";
IPAddress ip(172, 21, 64, 174); //Arduino IP address. Only used when DHCP is turned off.
//EthernetServer server(80);
EthernetClient client; //define 'client' as object
String data; //GET query with data
float suhu; //suhu (bahasa Indonesia) means temperature
boolean koneksi = false;
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
    //server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println(Ethernet.localIP());
  }
  delay(1000);
}
//------------------------------------------------------------------------------
void loop() {
  //EthernetClient client = server.available();  // try to get client

  sensors.requestTemperatures(); // Send the command to get temperature readings
  //Serial.print(sensors.getTempCByIndex(0));
  //int baca_ADC = analogRead(7); //read analog input on pin A7
  //suhu = baca_ADC * 5.0 * 100.0/1024.0; // ADC to celcius conversion
  suhu = sensors.getTempCByIndex(0);
  kemasData(); //packing GET query with data
  Serial.println("connecting...");

  while (!client.connect(website, 80)) {
    Serial.println("Can't get a connection!");
  }
  delay(1000);
  sendData();
  koneksi = true; //connected = true

  /*if (client.connect(website, 80)) {
    sendData();
    koneksi = true; //connected = true
    }
    else{
    Serial.println("connection failed");
    }*/

  // loop
  while (koneksi) {
    if (client.available()) {
      //Serial.print("Here");
      char c = client.read(); //save http header to c
      Serial.print(c); //print http header to serial monitor
    }
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      Serial.print("Temperature Sent :");
      Serial.println(suhu); //print sent value to serial monitor
      client.stop();
      while (client.connected())
      {
        client.stop();
        Serial.println('stopping the connection... ');
      }
      Serial.println('socket stop');

      koneksi = false;
      data = ""; //data reset
    }
  }
  delay(120000); // interval
}

void kemasData() {
  //data+="";
  data += "GET /pushingbox?devid=vA30422F00321904&tempData="; //GET request query to pushingbox API
  data += suhu;
  data += " HTTP/1.0";
  //data = "GET /pushingbox?devid=vA30422F00321904&tempData=24.06 HTTP/1.0";

  //Serial.print(data);
}
void sendData() {
  Serial.println("connected");
  client.println(data);
  delay(500);
  client.println("Host: api.pushingbox.com\r\n");
  client.println("Connection: close");
  client.println("User-Agent: Arduino");
  delay(500);
  client.println();
    while(client.connected()) {
     client.flush();
     client.stop();
     delay(1000);
     } 
  //client.println("GET /pushingbox?devid=vA30422F00321904&tempData=24.19 HTTP/1.0");
}


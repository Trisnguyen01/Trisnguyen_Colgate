,#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "max6675.h"
#include "DebugMacros.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20,16,2);
int thermoDO = D7;
int thermoCS = D6;
int thermoCLK = D5;
int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = D2;      // select the pin for the LED
int sensorValue = 0; 
// Fill ssid and password with your network credentials
const char* ssid = "Hai";
const char* password = "haitran12";

const char* host = "script.google.com";
const int httpsPort = 443;
const char *GScriptId = "AKfycbzUQ-vgBtHi20wzAggZyUBeQgl_Ot-dAFDKSRnFkKDZxUUUTxfr";

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?tag=Vibration&value=";

String payload = "";

HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation

const int analog_ip = D7; 
int inputVal  = 0;        
//Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//int vccPin = 3;
//int gndPin = 2;

  

void setup() {
  Serial.begin(115200);
  Wire.begin(2,0);
  lcd.clear();
  lcd.init();
  Serial.flush(); 
  

//  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
 // pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  
  //Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  //delay(500); 
  
    // use Arduino pins
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //Wire.begin(D1,D2);
  // flush() is needed to print the above (connecting...) message reliably, 
  // in case the wireless connection doesn't go through
 // Serial.flush();
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  
  payload = "tag=aaaa&value=122";
  client->POST(url, host, payload, false);
  client->GET(url, host); 
}

void loop() {
  static int connect_count = 0;
  static bool flag = false;
  

  //Serial.print("Ambient = "); 
  //inputVal = thermocouple.readCelsius();
  //Serial.print("C = "); 
  //Serial.println(thermocouple.readCelsius());
  //Serial.print(inputVal);
  //Serial.println("*C");
  
   sensorValue = analogRead(A0);
  // turn the ledPin on
  Serial.println(sensorValue);
  digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  delay(1000);
  // turn the ledPin off:
  digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  delay(1000);
  String myString = String(sensorValue);  
  String FinalStringToSend;
  FinalStringToSend = url + myString;

  
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
      payload= "";
       Serial.println("POST Data to Sheet");
//      FinalStringToSend = url + myString;
  Serial.println("POST url :" + FinalStringToSend);
  client->POST(FinalStringToSend, host, payload);
    }
  }
  else{
  Serial.println(" >> Failed to POST data");
  }
  Serial.println("GET url :"+FinalStringToSend);
  client->GET(FinalStringToSend, host);
  delay(1000);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("ColgatePalmolive");
  lcd.setCursor(4,1);
  lcd.print("Viet Nam");
  delay(10000);
  lcd.clear();
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.setCursor(6,0);
  lcd.print("n/a");
  lcd.setCursor(0,1);
  lcd.print("Vibration:");
  lcd.setCursor(11,1);
  lcd.print(sensorValue);
  lcd.setCursor(13,1);
  lcd.print("mm");
  delay(10000);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("IoT for Furture!");
  lcd.setCursor(4,1);
  lcd.print("T&E Area");                 
}

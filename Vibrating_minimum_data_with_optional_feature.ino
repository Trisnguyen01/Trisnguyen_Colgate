#include <Adafruit_MLX90614.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include "DebugMacros.h"
#include <Wire.h>
#include <FastIO.h>
//#include <I2CIO.h>
//#include <LCD.h>
//#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>
//#include <delay.h>
//#include "max6675.h"
// un-comment this line to print the debugging statements
#define GPIO_ADDR   0x27;
//#define idx = 5;
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
static bool flag = false;
int sensorPin = A0; 
int avg_sensorValue = 0, retval;
//int sensorValue=0;   
int sensorValue[5] ; 
int inputVal  = 0;        
String FinalStringToSend;

// Fill ssid and password with your network credentials
const char* ssid = "colgate";
const char* password = "colgate1";

const char* host = "script.google.com";
const char *GScriptId = "AKfycbw0779SdL4jUI8K22505iPBXHPS9EcmwWPHGWcMx1nMZwjt2tU"; //script full option
const int httpsPort = 443;

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?tag=Vibration&value=";

String payload = "";

HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation

//const int analog_ip = D7; 


  

void setup() {
  Serial.begin(115200);
  Wire.begin(2,0);
  //lcd.clear();
  //lcd.begin(16,2);
  Serial.flush(); 
  static bool flag = false;

  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid); //wifi name
  WiFi.begin(ssid, password);
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

/*/Delay function is built to support timer function
    void Delay(float delayTime, void (func)())
    {
        unsigned long endTime = millis() + delayTime;
        while(millis() < endTime)
        {
        func();
        while(millis() < endTime){};  //this line is to keep the func() is a infinity loop
        }
    } return endTime
  */
  
  //bool flag = false;
  // Try to connect for a maximum of 5 times
  trytoconnect();

//get rid of this step to reduce the work for arduino; no need to print those information on screen.
  /*if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }*/
  
  payload = "tag=aaaa&value=122";
  client->POST(url, host, payload, false);
  client->GET(url, host); 
}

void trytoconnect()
{
  for (byte i=0 ; i<5 ; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
}
  
void pushdata()
{
  for (byte i = 0 ; i < 5 ; i++ ){  
  
    sensorValue[i] = analogRead(A0) ;
    sensorValue[i] = sensorValue[i] * 990 / 1024 ;
    avg_sensorValue = ( avg_sensorValue + sensorValue[i]) / (i+1) ;
    delay(1000 * 35) ; //real 
    delay(1000 * 1) ; //test
                          }
  String myString = String(avg_sensorValue);  
  //Serial.println(sensorValue);
 // delay(1000);
 
    FinalStringToSend = url + myString;

  
  if (client != nullptr){
    if (!client->connected() ){
      client->connect(host, httpsPort);
      payload= "";
      Serial.println("POST Data to Sheet");
            //      FinalStringToSend = url + myString;
      Serial.println("POST url :" + FinalStringToSend);
      client->POST(FinalStringToSend, host, payload); //send data to google sheet POST function
                              }
                        } //end function pushdata
  
  
} //end of function push data                      

void TotalWork(){
    if (retval == 0) {
        trytoconnect();
        pushdata();   
        delay(1000 * 60);
        delay(1000 * 1) ; //test    
  
                    }
    else { 
        //for (byte i = 0 ; i < 5 ; i++ ){
        pushdata();
        delay(1000 * 60);
        delay(1000 * 1) ; //test                
          }
 
                } //end of function TotalWork
                
void loop() {
  /*  if (retval == 0) {
          trytoconnect();
          for (byte i = 0; i < 5; i++ ){
              pushdata();        }
                         }
    else { 
    pushdata();
    delay(1800000);}*/
   //for (byte i = 0 ; i < 3 ; i++)
      
      TotalWork();
  
  
  //Serial.println("GET url :"+FinalStringToSend);
  client->GET(FinalStringToSend, host);
  //delay(10000);
 
  
  
  
  
  
  
  
  
  /*lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("ColgatePalmolive");
  lcd.setCursor(4,1);
  lcd.print("Viet Nam");
  //delay(1000);
  lcd.clear();
  //delay(1000);
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
  //delay(1000);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("IoT for Furture!");
  lcd.setCursor(4,1);
  lcd.print("T&E Area");                 
  //delay(20000);*/
}

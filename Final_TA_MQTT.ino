#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
 

const char* ssid = "WIFI farizpd";                   // wifi ssid
const char* password =  "ooaisee1";         // wifi password
const char* mqttServer = "192.168.100.15";    // IP adress Raspberry Pi
const int mqttPort = 1883;
const char* mqttUser = "";      // if you don't have MQTT Username, no need input
const char* mqttPassword = "";  // if you don't have MQTT Password, no need input        
const char* myTopik = "halte"; 

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);  
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.println("Connecting to WiFi..");
  }
  //Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    //Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      //Serial.println("connected");
      for(int i =0 ;i < 3;i++){
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
          delay(500);                       // wait for half a second
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED off 
          delay(500);        
        }

    } else {

      //Serial.print("failed with state ");
      //Serial.print(client.state());
      delay(2000);

    }
  }

//  client.publish("esp8266", "Hello Raspberry Pi");
//  client.subscribe("esp8266");

}

void callback(char* topic, byte* payload, unsigned int length) {
uint8_t textInput[5];  
    
 
  for (int i = 0; i < 10; i++) { //jadi kan ini max 5 huruf, biar gampang kalo ada input 4 huru
      //Serial.print((char)payload[i]); //dibikin jadi 5 dimana huruf ke 5 itu "z"
      textInput[i]=(char)payload[i];
      if(i>=length){
        textInput[i]='Z';
        }
  }   
  String str((char*)textInput);  
  //Serial.println(str);
 
   if(str=="redZZZZZZZ"){//note, harus 5 huruf karena arraynya di set 5 
      //red  
        Serial.write(210);  
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off  
        delay(1000);
    }
  if(str=="yellowZZZZ"){ 
      //yellow  
        Serial.write(211);  
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off  
        delay(1000);
    }
  if(str=="greenZZZZZ"){ 
      //green 
        Serial.write(212);  
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off  
        delay(1000);
  } 
  if(str=="blueZZZZZZ"){ 
      //blue 
        Serial.write(213);  
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off  
        delay(1000); 
    }
    
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on   
}

void loop() {
  //publish
  //client.publish(myTopik, "Hello Raspberry Pi"); 
    client.subscribe(myTopik);
    delay(300);
    client.loop(); 
}

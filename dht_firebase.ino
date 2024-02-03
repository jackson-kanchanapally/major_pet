#include "DHT.h"
#define DHTPIN D1
#define DHTTYPE DHT11

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

DHT dht(DHTPIN, DHTTYPE);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ACT101511044984"
#define WIFI_PASSWORD "39197540"

// Insert Firebase project API Key
#define API_KEY "AIzaSyALcg6utMHov-wQwTfgqzUTSetnbd9AKFM"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://major-757a9-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;

void setup(){
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
 delay(1000);
  float h = dht.readHumidity();

  float t = dht.readTemperature();
  float ft=(t*9/5)+32;
  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.setString(&fbdo, "DHT/humidity", String(h))){
       Serial.print("Humidity: ");
       Serial.println(h);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Write a Float number on the database path test/float
    if (Firebase.RTDB.setString(&fbdo, "DHT/temperature", String(ft))){
       Serial.print("Temperature: ");
       Serial.println(ft);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("______________________________");
}

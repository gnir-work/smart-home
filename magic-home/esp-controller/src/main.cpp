#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

/************************* Credentials *********************************/

#include "wifi_credentials.h"

/****************************** Consts ***************************************/

String POWER_URL = "http://192.168.1.27:8000/power";
int SOUND_PIN = A0;
int WIFI_STATUS_PIN = 4;
int ACTION_PIN = 5;
int SYNC_INTERVAL = 30;

/****************************** Global State ***************************************/

bool is_on = false;
int query_state_counter = 0;
WiFiClient wifiClient;

/****************************** Code ***************************************/

void connect_to_wifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.disconnect();
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(WIFI_STATUS_PIN, LOW);
    delay(500);
    Serial.print(".");
    digitalWrite(WIFI_STATUS_PIN, HIGH);
    delay(500);
  }

  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);

  pinMode(WIFI_STATUS_PIN, OUTPUT);
  pinMode(ACTION_PIN, OUTPUT);

  connect_to_wifi();

  Serial.println("setup was done");
}

void update_current_light_status()
{
  HTTPClient http;                                          //Declare object of class HTTPClient
  http.begin(wifiClient, POWER_URL); //Specify request destination
  int httpCode = http.GET();                                //Send the request
  if (httpCode > 0)
  {
    String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);

    if (JSON.typeof(myObject) == "undefined")
    {
      Serial.println("Parsing input failed!");
      return;
    }

    is_on = !!myObject["is_on"];

    if (is_on)
    {
      Serial.println("Power is on!");
    }
    else
    {
      Serial.println("Power is off!");
    }
  }
  else
  {
    Serial.println("Error in http request!");
  }

  http.end(); //Close connection
}

void toggle_light()
{
  digitalWrite(ACTION_PIN, HIGH);
  String data;
  if (is_on)
  {
    data = "{\"is_on\":false}";
  }
  else
  {
    data = "{\"is_on\":true}";
  }

  HTTPClient http;                                          //Declare object of class HTTPClient
  http.begin(wifiClient, POWER_URL); //Specify request destination
  http.addHeader("Content-Type", "application/json");
  http.POST(data);

  String payload = http.getString();
  JSONVar myObject = JSON.parse(payload);

  if (JSON.typeof(myObject) == "undefined")
  {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("Response from light toggle=");
  Serial.println(myObject);
  if (myObject["success"])
  {
    Serial.println("Device toggled successfully!");
    is_on = !is_on;
  }
  else
  {
    Serial.println("Failed to toggle device!");
  }

  http.end(); //Close connection
  digitalWrite(ACTION_PIN, LOW);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connect_to_wifi();
  }
  else
  {
    int sensor_value = analogRead(SOUND_PIN);

    if (sensor_value > 49)
    {
      Serial.print("Triggering light toggle after hearing=");
      Serial.println(sensor_value, DEC);
      toggle_light();
      Serial.println("Done");
      delay(1000);
    }

    if (query_state_counter * SYNC_INTERVAL * 1000 < millis())
    {
      Serial.print("The current power state is: ");
      Serial.println(is_on);
      Serial.println("Querying for devices state...");
      update_current_light_status();
      Serial.println("Done");
      query_state_counter += 1;
    }
  }
}
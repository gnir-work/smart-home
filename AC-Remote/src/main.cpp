/************************* External Libraries *********************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Amcor.h>

/************************* Credentials *********************************/
#include "wifi_credentials.h"
#include "adafruit_credentials.h"


/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

/************ Global State ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

// Use GPIO D2 for IR led
const int IR_GPIO = 4;

// Create IR Remote for Amcor device types
IRAmcorAc iramcor(IR_GPIO);

// Remote initial state - Power: Off, Mode: 1 (Cool), Fan: 4 (Auto), Temp: 22C, Max: Off
const uint8_t initial_ac_state[] = {0x01, 0x41, 0x2C, 0x00, 0x00, 0xC0, 0xC0, 0x2C};

/****************************** Consts ***************************************/
#define ON_STATE "ON"
#define OFF_STATE "OFF"


/****************************** Feeds ***************************************/

// Setup a feed called 'time' for subscribing to current time
//Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&mqtt, "time/seconds");

// Setup a feed called 'slider' for subscribing to changes on the slider
Adafruit_MQTT_Subscribe temperature_subscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ac-temperature", MQTT_QOS_1);

// Setup a feed called 'onoff' for subscribing to changes to the button
Adafruit_MQTT_Subscribe power_subscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ac-onoff", MQTT_QOS_1);

Adafruit_MQTT_Publish power_publish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ac-onoff", MQTT_QOS_1);

Adafruit_MQTT_Publish temperature_publish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ac-temperature", MQTT_QOS_1);

Adafruit_MQTT_Publish logs_publish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ac-remote-logs", MQTT_QOS_1);

/*************************** Sketch Code ************************************/
void log(char *message) {
  Serial.println(message);
  logs_publish.publish(message);
}


void on_temperature_change(double temperature) {
  Serial.print("Temperature change callback was called with: ");
  Serial.println(temperature);
  iramcor.setTemp(temperature);
  if (iramcor.getPower()) {
    log("The remote is on, applying the temperature");
    iramcor.send();
  } else {
    log("The remote is off, updating state but not applying temperature");
  }
}

void on_power_change(char *data, uint16_t len) {
  Serial.print("Power change callback was called with: ");
  Serial.println(data);
  if (strcmp(data, ON_STATE)) {
    Serial.println("Turning AC off..");
    iramcor.off();
    iramcor.send();
  } 
  if (strcmp(data, OFF_STATE)) {
    Serial.println("Turning AC on..");
    iramcor.on();
    iramcor.send();
  }
}

/**
 * Update the adafruit broker with the current state as known to the microcontroller.
 * This is useful in order to handle resets of the micro controller. 
 */
void update_adafruit_broker() {
  Serial.print("POWER: ");
  if (iramcor.getPower()) {
    Serial.println(ON_STATE);
    power_publish.publish(ON_STATE);
  } else {
    Serial.println(OFF_STATE);
    power_publish.publish(OFF_STATE);
  }

  uint8_t temperature = iramcor.getTemp();
  Serial.print("Temperature: ");
  Serial.println(temperature);
  temperature_publish.publish(temperature);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  temperature_subscribe.setCallback(on_temperature_change);
  power_subscribe.setCallback(on_power_change);
  
  mqtt.subscribe(&temperature_subscribe);
  mqtt.subscribe(&power_subscribe);

  // Initialize remote state
  iramcor.begin();
  iramcor.calibrate();
  iramcor.setRaw(initial_ac_state);

  // Connect to adafruit
  MQTT_connect();
  
  // Update broker of initial controller state
  log("Controller was turned on!");
  log("Updating the adafruit broker with initial controller state");
  update_adafruit_broker();

}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}
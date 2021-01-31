/************************* External Libraries *********************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <string.h>

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

/****************************** Consts ***************************************/
const int STATE_TO_GPIO_LENGTH = 3;
const int STATE_TO_GPIO[STATE_TO_GPIO_LENGTH] = {12, 14, 16};

/****************************** Feeds ***************************************/

// Setup a feed called 'time' for subscribing to current time
//Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&mqtt, "time/seconds");

// Setup a feed called 'slider' for subscribing to changes on the slider
Adafruit_MQTT_Subscribe power_level_subscribe = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fan-power", MQTT_QOS_1);


Adafruit_MQTT_Publish logs_publish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fan-remote-logs", MQTT_QOS_1);

/*************************** Sketch Code ************************************/
void log(char *message) {
  Serial.println(message);
  logs_publish.publish(message);
}


void on_power_level_change(double power_level) {
  const int power_level_int = (int) power_level;

  if (power_level_int < 0 || power_level_int > 3) {
    log("Invalid power level received: ");
    log(power_level_int);
    return;
  } else {
    log("Changing power level");
  }

  Serial.println("Powering off all of the GPIOs");
  for (int i = 0; i < STATE_TO_GPIO_LENGTH; i++) {
    digitalWrite(STATE_TO_GPIO[i], HIGH);
  }

  if (power_level_int > 0) {
    const int gpio = STATE_TO_GPIO[power_level_int - 1];
    Serial.print("Powering on GPIO: ");
    Serial.println(gpio);
    digitalWrite(gpio, LOW);
  }
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

  // Set all GPIOs to output
  for (int i = 0; i < STATE_TO_GPIO_LENGTH; i++) {
    pinMode(STATE_TO_GPIO[i], OUTPUT);
    digitalWrite(STATE_TO_GPIO[i], HIGH);
  }

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

  power_level_subscribe.setCallback(on_power_level_change);
  
  mqtt.subscribe(&power_level_subscribe);

  // Connect to adafruit
  MQTT_connect();
  
  // Update broker of initial controller state
  log("Controller finished initialization!");

  delay(200);
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
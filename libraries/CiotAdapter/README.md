CIOT Client Library
===================

This is the arduino library to easy access CIOT[http://api.iot.ciandt.com]

Just download the zip structure and import to your arduino installation.

## Example
### Simple example to sender data
```js
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <CiotAdapter.h>

// Specify the number of rows in your array, in this case 3
const int numElements=3;

// Define the MAC Address
byte *mac;

CiotAdapter adapter;
const char* ssid     = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";


void setup() {
  // Start serial port
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  mac = WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.println(getMacAddress());
}

void loop() {
  // Define the values of the array to be sent
  String result;
  String array[numElements][2] =
    {
      {"Name", "James"},
      {"Age", "28"},
      {"Gender", "Male"}
    };

  result = adapter.sendStream(array, numElements, mac);

    Serial.println(result);
  //Send this stream every 10 secs.
  delay(10000);
}
```

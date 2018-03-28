/*
 *  Autor: Carlos Eduardo (kadu@kadu.com.br)
 *  Libraries: WifiManager, Adafruit Unified Sensors, Adafruit DHT, CiotAdapeter (on libraries directory)
 *  Dashboard:  Freeboard.io (https://freeboard.io/board/gchlG9)
*/
#include <CiotAdapter.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            D2
#define DHTTYPE           DHT22
#define ESP_VERSION       0     //Version of firmware, needed to initialize CiotAdapter

const int CONFIG_PORT = 0;
const int STATUS_PORT = LED_BUILTIN;
const int numElements=2;
unsigned long lastUpdate = 0;
unsigned long delayms = 60000;

DHT_Unified dht(DHTPIN, DHTTYPE);
CiotAdapter adapter(ESP_VERSION, CONFIG_PORT, STATUS_PORT); // HTTP

void sendTemperature() {
  float temp, humidity;
  temp = humidity = 0.0;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
    temp = -99.9;
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
    temp = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
    humidity = -99.9;
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
    humidity = event.relative_humidity;
  }

  String s_temp, s_humidity;
  s_temp = String(temp);
  s_humidity = String(humidity);


  // Define the values of the array to be sent
  String result;
  String array[numElements][2] =
  {
    {"Temperature", s_temp.c_str()},
    {"Humidity", s_humidity.c_str()}
  };

  result = adapter.sendStream(array, numElements);

  Serial.println(result);
  lastUpdate = millis();
}

void setup() {
  Serial.begin(115200);
  adapter.initialize();

  sendTemperature();

}

void loop() {
  if (!adapter.processConfigButton()) {
    // config button pressed
  }

  if (adapter.hasConnection()) {
    //
  } else { // when connection is lost, timer needs to be reseted
    //
  }

  if(millis() > (lastUpdate + delayms)) {
    sendTemperature();
  }
}

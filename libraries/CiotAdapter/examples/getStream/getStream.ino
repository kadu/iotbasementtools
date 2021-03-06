#include <CiotAdapter.h>

#define ESP_VERSION 0 //Version of firmware, needed to initialize CiotAdapter

const int CONFIG_PORT = 0;
const int STATUS_PORT = LED_BUILTIN;

CiotAdapter adapter(ESP_VERSION, CONFIG_PORT, STATUS_PORT); // HTTP

// Specify the number of rows in your array, in this case 3
const int numElements=3;

void setup() {
  // Start serial port
  Serial.begin(115200);

  adapter.initialize();

  getStream();
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
}

void getStream() {
  String result = adapter.getStream();
  Serial.println(result);
}

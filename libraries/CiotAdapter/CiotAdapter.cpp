#include "CiotAdapter.h"

extern "C" {
  #include "user_interface.h"
}


#define UP 0
#define DOWN 1

const char CiotAdapter::POST_METHOD[] = "POST";
const char CiotAdapter::GET_METHOD[] = "GET";
const char CiotAdapter::PUT_METHOD[] = "PUT";
const char CiotAdapter::DELETE_METHOD[] = "DELETE";
const char CiotAdapter::CHAVE = '{';
const char CiotAdapter::ASPAS_DP[] = "\"";
const char CiotAdapter::STR_EMPTY[] = "";
const short CiotAdapter::V1 = 10;
const short CiotAdapter::V2 = 20;
const short CiotAdapter::V2_TST = 22;
const short CiotAdapter::V2_STAGE = 30;
const short CiotAdapter::V2_PROD = 33;
const short CiotAdapter::V2_EUROPE = 30;

int millisAtual = 0;

const int CiotAdapter::CB_AP_PROVISIONING = 3;
const int CiotAdapter::CB_OTA_UPDATE = 5;
const int CiotAdapter::CB_DEBUG_MODE = 7;
const int CiotAdapter::CB_SMART_CONFIG = 11;
const int CiotAdapter::CB_CONNECT_TO_WIFI = 13;

int CiotAdapter::options = CB_AP_PROVISIONING*CB_OTA_UPDATE*CB_DEBUG_MODE*CB_SMART_CONFIG*CB_CONNECT_TO_WIFI; // enable all options to configButton
char* CiotAdapter::server;
int CiotAdapter::port;
bool CiotAdapter::secure;
const char* CiotAdapter::dataEndpoint;
const char* CiotAdapter::versionEndpoint;
log_callback CiotAdapter::logFunction;
bool CiotAdapter::_VERBOSE_DEBUG;

int CiotAdapter::_configButtonPin; //D2 in NodeMCU
int CiotAdapter::_statusLedPin; //D7 in NodeMCU
int CiotAdapter::_statusLedState = LOW;

int CiotAdapter::_statusLedPin_Red;
int CiotAdapter::_statusLedPin_Green;
int CiotAdapter::_statusLedPin_Blue;
int CiotAdapter::_statusLed_HasRGB;

int CiotAdapter::_fw_version;
int CiotAdapter::_currentButtonState;
int CiotAdapter::_buttonSecondsCount;   // How long the button was held (secs)
int CiotAdapter::_errorsCount =0;   // Number of fails in connection try
byte CiotAdapter::_previousButtonState = LOW;
unsigned long CiotAdapter::_buttonFirstTime;   // how long since the button was first pressed


// constants for min and max PWM
const int minPWM = 0;
const int maxPWM = 1023;
byte fadeDirection = UP;
int fadeValue = 0;
byte fadeIncrement = 5;
unsigned long previousFadeMillis;
int fadeInterval = 10;

WiFiClient CiotAdapter::client;
WiFiClientSecure CiotAdapter::client_s;
WiFiManager CiotAdapter::wifiManager;

void CiotAdapter::_init(short apiVersion) {
  _statusLed_HasRGB = 0;
  if (apiVersion == V1) {
    CiotAdapter::server = "api.iot.ciandt.com";
    CiotAdapter::port = 80;
    CiotAdapter::secure = false;
    CiotAdapter::dataEndpoint = "/v2/data";
    CiotAdapter::versionEndpoint = "/v2/firmware";
  } else
  if ((apiVersion == V2) || (apiVersion == V2_PROD)) {
    CiotAdapter::server = "prod-dot-ciot-garage.appspot.com";
    CiotAdapter::port = 443;
    CiotAdapter::secure = true;
    CiotAdapter::dataEndpoint = "/_ah/api/iotPlatform/v2/data";
    CiotAdapter::versionEndpoint = "/_ah/api/iotPlatform/v2/firmware/ESP/last";
  } else
  if (apiVersion == V2_TST) {
    CiotAdapter::server = "tst-dot-ciot-garage.appspot.com";
    CiotAdapter::port = 443;
    CiotAdapter::secure = true;
    CiotAdapter::dataEndpoint = "/_ah/api/iotPlatform/v2/data";
    CiotAdapter::versionEndpoint = "/_ah/api/iotPlatform/v2/firmware/ESP/last";
  } else
  if (apiVersion == V2_STAGE) {
    CiotAdapter::server = "mirror-dot-iot-platform-eu-dev.appspot.com";
    CiotAdapter::port = 443;
    CiotAdapter::secure = true;
    CiotAdapter::dataEndpoint = "/_ah/api/iotPlatform/v2/data";
    CiotAdapter::versionEndpoint = "/_ah/api/iotPlatform/v2/firmware/ESP/last";
  } else
  if (apiVersion == V2_EUROPE) {
	  //https://mirror-dot-iot-platform-eu-dev.appspot.com
    CiotAdapter::server = "mirror-dot-iot-platform-eu-dev.appspot.com";
    CiotAdapter::port = 443;
    CiotAdapter::secure = true;
    CiotAdapter::dataEndpoint = "/_ah/api/iotPlatform/v2/data";
    CiotAdapter::versionEndpoint = "/_ah/api/iotPlatform/v2/firmware/ESP/last";
  }
  ledsOff();
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, short apiVersion) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  _init(apiVersion);
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, short apiVersion, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  _init(apiVersion);
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed) {
  _fw_version = fw_version;
  logFunction = f;
  _configButtonPin = PinConfigButton;
  _statusLedPin = PinStatusLed;
  _init(V2);
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
}

void defaultDebug() {}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, short apiVersion) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  _init(apiVersion);
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, short apiVersion, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  _init(apiVersion);
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, bool _secure) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  secure = _secure;
  if (_secure) {
    port = 443;
  }
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  secure = _secure;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  secure = _secure;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, bool _secure) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  secure = _secure;
  if (_secure) {
    port = 443;
  }
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  secure = _secure;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  secure = _secure;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, bool _secure, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  secure = _secure;
  if (_secure) {
    port = 443;
  }
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  secure = _secure;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint, int _options) {
  CiotAdapter(fw_version, f, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  secure = _secure;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, bool _secure, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  secure = _secure;
  if (_secure) {
    port = 443;
  }
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  secure = _secure;
  options = _options;
}

CiotAdapter::CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint, int _options) {
  CiotAdapter(fw_version, defaultDebug, PinConfigButton, PinStatusLed);
  server = _server;
  port = _port;
  dataEndpoint = _dataEndpoint;
  versionEndpoint = _versionEndpoint;
  secure = _secure;
  options = _options;
}

bool CiotAdapter::initialize() {
  Serial.println(F("Starting CiotAdapter library..."));
  Serial.print(F("Environment: "));
  if (secure) {
    Serial.print("https://");
  } else {
    Serial.print("http://");
  }
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);
  Serial.print(F("MAC Address: "));
  Serial.println(getMacAddress());
  startOTAUpdate();

  ledsOff();

  if (hasConnection()) {
	  ledsOff();
	  //digitalWrite(13, LOW);
    return true;
  } else {

	  ledsOff();
    connectToWiFi();
    return hasConnection();
  }
}

bool CiotAdapter::hasConnection() {
  // fabiohsv: evitar logs neste método pois se usado no loop pode gerar muito log no console
  if (WiFi.status() != WL_CONNECTED)
    return false;
  else
    return true;
}

int CiotAdapter::processConfigButton() {
  _currentButtonState = digitalRead(_configButtonPin);

  ledsOff();

  if (_currentButtonState == LOW && _previousButtonState == HIGH && millis()- _buttonFirstTime > 200) {
    _buttonFirstTime = millis();    // if the buttons becomes press remember the time
    Serial.println(F("config button pressed"));
  }

  if (_currentButtonState == LOW && ((millis() - _buttonFirstTime) % 1000) < 20 && millis() - _buttonFirstTime > 500) {
    pinMode(_statusLedPin, OUTPUT);
    //digitalWrite(12, LOW);
    digitalWrite(_statusLedPin_Red, LOW);
    delay(300);
    //digitalWrite(12, HIGH);
    digitalWrite(_statusLedPin_Red, HIGH);
    _buttonSecondsCount++;
    Serial.println(_buttonSecondsCount);
    //blinkStatusLed(1, 100);
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=6 && _buttonSecondsCount < 7 && (options%CB_AP_PROVISIONING) == 0) {
    _buttonSecondsCount = 0;
    ledsOff();
    Serial.println(F("startApProvisioning"));

    millisAtual = millis();
    while((millis() - millisAtual) < 3000){
      //fadeLed(13);
      fadeLed(_statusLedPin_Green);
      ledsOff();
      wdt_disable();
    }

    ledsOff();
    startApProvisioning();
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=5 && _buttonSecondsCount < 6 && (options%CB_OTA_UPDATE) == 0) {
    _buttonSecondsCount = 0;
    Serial.println(F("Starting OTA Update"));
    ledsOff();
    millisAtual = millis();

    while((millis() - millisAtual) < 3000){
      fadeLed(_statusLedPin_Red);
      fadeLed(_statusLedPin_Green);
      //fadeLed(_statusLedPin_Blue);
      wdt_disable();
    }

    analogWrite(_statusLedPin_Red, 0);
    analogWrite(_statusLedPin_Green, 0);
    analogWrite(_statusLedPin_Blue, 0);

    ledsOff();
    startOTAUpdate();
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=4 && _buttonSecondsCount < 5 && (options%CB_DEBUG_MODE) == 0) {
    _buttonSecondsCount = 0;
    Serial.println(F("Debug Mode"));
    ledsOff();
    millisAtual = millis();

    while((millis() - millisAtual) < 3000){
    //fadeLed(12);
    fadeLed(_statusLedPin_Red);
    wdt_disable();
    }
    ledsOff();
    analogWrite(_statusLedPin_Red, 0);
    (*logFunction)();
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=3 && _buttonSecondsCount < 4 && (options%CB_SMART_CONFIG) == 0) {
    _buttonSecondsCount = 0;
    ledsOff();
    Serial.println(F("startSmartConfig"));
    millisAtual = millis();

    while((millis() - millisAtual) < 3000){
      fadeLed(_statusLedPin_Green);
      fadeLed(_statusLedPin_Blue);
      wdt_disable();
    }
    analogWrite(_statusLedPin_Green, 0);
    analogWrite(_statusLedPin_Blue, 0);
    startSmartConfig();
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=2 && _buttonSecondsCount < 3) {
    _buttonSecondsCount = 0;
  }

  if (_currentButtonState == HIGH && _previousButtonState == LOW && _buttonSecondsCount >=1 && _buttonSecondsCount < 2 && (options%CB_CONNECT_TO_WIFI) == 0) {
    _buttonSecondsCount = 0;
    ledsOff();
    connectToWiFi();
  }

  if (_currentButtonState == HIGH) {
    _buttonSecondsCount = 0;
  }

  _previousButtonState = _currentButtonState;
  ledsOff();

  return _currentButtonState;
}

void CiotAdapter::startSmartConfig() {
  //WiFi.disconnect();

  // set for STA mode
  WiFi.mode(WIFI_STA);

  // led status at pin16
  pinMode(_statusLedPin,OUTPUT);


  // if wifi cannot connect start smartconfig
  do{
    delay(500);
    WiFi.beginSmartConfig();
    while(1) {
      wdt_disable();
      delay(500);
      if(WiFi.smartConfigDone()) {
        Serial.println(F("SmartConfig Success"));
        ledsOff();
        break;
      }
    }
  }while(WiFi.status() != WL_CONNECTED);

  Serial.println(STR_EMPTY);
  Serial.println(STR_EMPTY);

  WiFi.printDiag(Serial);

  // Print the IP address
  ledsOff();
  Serial.println(WiFi.localIP());

  return;
}

void CiotAdapter::startApProvisioning() {
  ledsOff();
  String thingName = getMacAddress().substring(12);
  thingName.replace(":","");
  thingName.toUpperCase();
  thingName = "THING_" + thingName;

  wifiManager.startConfigPortal(thingName.c_str(), thingName.c_str());
  Serial.println(F("Connected..."));
  Serial.println(WiFi.localIP());
}

void CiotAdapter::turnOnStatusLed() {
  _statusLedState = HIGH;
  digitalWrite(_statusLedPin, HIGH);
}

void CiotAdapter::turnOffStatusLed() {
  _statusLedState = LOW;
  digitalWrite(_statusLedPin, LOW);
  ledsOff();
}

void CiotAdapter::blinkStatusLed(int times, int lengthms) {
  for (int x=0; x<times;x++) {
    digitalWrite(_statusLedPin, HIGH);
    delay(lengthms);
    digitalWrite(_statusLedPin, LOW);
    delay(lengthms);
  }
}

void CiotAdapter::connectToWiFi() {
  Serial.println(F("Entered Connect Function"));

  while (WiFi.status() == WL_CONNECTED) {
    delay(300);
  }

  Serial.println(F("Waiting the connection"));
  while (WiFi.status() != WL_CONNECTED) {
    if (_errorsCount++ > 30) {
      return;
    } else {
      if (processConfigButton() == LOW) {
        return;
      }
      Serial.print(F("."));
      delay(300);
    }

    Serial.println(F("You're connected to the network"));
    Serial.println(F("Waiting for an ip address"));

    while (WiFi.localIP() == INADDR_NONE) {
      if (processConfigButton() == LOW)
        return;
      Serial.print(F("."));

      delay(150);
    }
    Serial.println(F("IP Address obtained"));
    Serial.print(F("SSID: "));
    Serial.println(WiFi.SSID());

    // print your WiFi IP address:
    Serial.print(F("IP Address: "));
    Serial.println(getIpAddress());

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print(F("signal strength (RSSI):"));
    Serial.print(rssi);
    Serial.println(F(" dBm"));
    ledsOff();
    return;
  }
}

String CiotAdapter::sendStream(String array[][2], int elements) {
  return sendFullStream(array, elements).responseText;
}

HttpResponse CiotAdapter::sendFullStream(String array[][2], int elements) {
  String data[7][2] =
  {
    {F("content"), arrayToJson(array, elements, true)},
    {F("uptime"), String(millis())},
    {F("battery"), getVdd()},
    {F("firmwareVersion"), ASPAS_DP + String(_fw_version) + ASPAS_DP},
    {F("ipAddress"), ASPAS_DP + getIpAddress() + ASPAS_DP},
    {F("ssid"), ASPAS_DP + WiFi.SSID() + ASPAS_DP},
    {F("rssi"), ASPAS_DP + String(WiFi.RSSI()) + ASPAS_DP}
  };

  return httpRequest(server, port, secure, POST_METHOD, dataEndpoint, data, 7, false);
}

String CiotAdapter::getStream(String deviceId) {
  return getFullStream(deviceId).data;
}
String CiotAdapter::getStream() {
  return getStream(getMacAddress());
}

HttpResponse CiotAdapter::getFullStream(String deviceId) {
  String endpoint = String(dataEndpoint) + F("/") + deviceId + F("/last");
  return  httpRequest(server, port, secure, GET_METHOD, endpoint, true);
}

HttpResponse CiotAdapter::getFullStream() {
  return getFullStream(getMacAddress());
}

HttpResponse CiotAdapter::getCurrentVersion() {
  if (String(versionEndpoint).endsWith(F("ESP/last"))) {
    return httpRequest(server, port, secure, GET_METHOD, versionEndpoint, true);
  } else {
    String endpoint = String(dataEndpoint) + F("/") + getMacAddress() + F("/last");
    HttpResponse response = httpRequest(server, port, secure, GET_METHOD, endpoint, true);
    if (response.statusCode == 200) {
      String bigTableId = findText(response.data, F("\"bigTableId\":\""));
	  String endpoint = String(versionEndpoint) + F("/") + bigTableId + F("/ESP");
      return httpRequest(server, port, secure, GET_METHOD, endpoint, true);
    } else {
      return response;
    }
  }
}

String CiotAdapter::getMacAddress() {
  String macString = String(STR_EMPTY);
  byte mac[6];
  WiFi.macAddress(mac);
  for(int i = 0; i < 6; i++) {
    String hex = String(mac[i],HEX);
    if(hex.length() == 1)
      hex = "0" + hex;
    macString += hex ;
    if(i < 5)
      macString += ":";
  }
  return macString;
}

String CiotAdapter::getIpAddress() {
  IPAddress ip  = WiFi.localIP();
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return myIpString;
}

String CiotAdapter::getVdd() {
  int nowVolt = analogRead(A0);
  float vcc = (float)nowVolt*4/935;
  Serial.print("Math: ");
  Serial.println(vcc);

  Serial.print("Percent: ");
  int vdd2 = (float)(vcc - 3.2)*1000/12;//Setting 3.2 as 0% and 4.4 (3.2 + 12(/10)) as 100%
  Serial.println(vdd2);

  return (String) vdd2;
}

String CiotAdapter::findText(String text, String startText) {
  int startTextLength = startText.length();
  int first, last; //Variables to identify the beggining and the end of the texted searched
  boolean startToCopy = false;
  String endText = F("\"");
  int endTextLength = endText.length();
  int currentToCompare = 0;
  String response = String(STR_EMPTY);

  for (int i = 0; i < text.length(); i++) {

    if (!startToCopy) {

      if (text[i] == startText[currentToCompare]) { //If the first letter matches
        if (++currentToCompare == startTextLength)//if the following letters match, until the last one (lenght)
        {
          first = i + 1; //Sets the beggining of the word searched
          startToCopy = true; //We can copy the word between the start and end strings
          currentToCompare = 0;  //Set the number of matched Chars as 0 again
        }
      }
      else {
        currentToCompare = 0; //If not all the letters are equal, set to 0 again
      }
    }
    else
    {
      currentToCompare = 0; //If not all the letters are equal, set to 0 again
      if (text[i] == endText[currentToCompare])
        if (++currentToCompare == endTextLength)
        {
          last = i;
          startToCopy = false;
          break;
        }
    }
  }
  for (int x = first; x < last; x++)
    response += text[x];
  return response;
}

bool CiotAdapter::startOTAUpdate() {
  if (!hasConnection()) {
    connectToWiFi();
  }

  HttpResponse response = getCurrentVersion();
  if (response.statusCode == 200) {
    String OTA_str_version = findText(response.data, F("\"version\":\""));
	if (OTA_str_version == "") {
      OTA_str_version = findText(response.data, F("\"version\": "));
	  OTA_str_version = OTA_str_version.substring(0, OTA_str_version.indexOf(','));
    }

    int OTA_version =  OTA_str_version.toInt();

    String bucket = findText(response.data, F("storage.googleapis.com"));

    return startOTAUpdate(OTA_version, bucket);
  } else {
    Serial.println(F("ERROR: Version was not found"));
    return false;
  }
}

bool CiotAdapter::startOTAUpdate(int OTA_version, String bucket) {
  Serial.println(F("Start OTA Update..."));
  Serial.print(F("FW Version: "));
  Serial.println(_fw_version);
  Serial.print(F("OTA Version: "));
  Serial.println(OTA_version);

  if (OTA_version >= 0) {
    if (_fw_version == OTA_version) {
      Serial.println(F("Version is up-to-date!"));
    } else
    /*if (_fw_version > OTA_version) {
      Serial.println(F("Something went wrong... Check the fw_version at Arduino code or the online versioning"));
    } else
    if (_fw_version < OTA_version) */
    {
      Serial.println(F("Version is outdated, performing update..."));

      Serial.println(F("bucket:"));
      Serial.println(bucket);

      for (int i=0; i < 5; i++) {

        Serial.print(F("OTA Attempt number "));
        Serial.println(i+1);

        t_httpUpdate_return ret = ESPhttpUpdate.update(F("storage.googleapis.com"), 80, bucket);

        Serial.println(F("SWITCH:"));

        switch(ret) {
          case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

          case HTTP_UPDATE_NO_UPDATES:
            Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
            break;

          case HTTP_UPDATE_OK:
            Serial.println(F("HTTP_UPDATE_OK"));
            return true;

          default: {
            Serial.println(F("NO STATUS FOUND..."));
            Serial.println(ret);
          }
        }
        Serial.println(F("Some errors occurred"));
        delay(2000);
      }
    }
  }
  return false;
}

/*
http request utils
*/

/*
Converte array de string em um texto no formato json
asStr: indica se deverá ser adicionado aspas no value
*/
String CiotAdapter::arrayToJson(String array[][2], int size, bool asStr) {
  String key;
  String value;
  String json = String(CHAVE);

  String aspas = STR_EMPTY;
  if (asStr) {
    aspas = ASPAS_DP;
  }

  for (int i=0; i<size;i++) {
    key = array[i][0];
    value = array[i][1];
    json += ASPAS_DP + key + F("\":");
    json += aspas + value + aspas;
    if (size > 1 && i != size-1) {
      json += F(",");
    }
  }

  return json+F("}");
}

bool defaultRead(char c, HttpResponse* response) {
  return true;
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming) {
  HttpResponse response;
  Serial.println(F("trying connecting..."));
  if (WiFi.status() != WL_CONNECTED || !_connect(_server, _port, _secure)) {
    if (WiFi.status() != WL_CONNECTED) {
      response.responseText = F("No Internet Connection");
    } else {
      response.responseText = F("Server not found");
    }
    response.statusCode = 0;
  } else {

    if (_secure) {
      Serial.println(F("connected with secure"));
    } else {
      Serial.println(F("connected"));
    }

    _print(_method, _secure);
    _print(F(" "), _secure);
    _print(endpoint, _secure);
    _println(F(" HTTP/1.1"), _secure);
    _print(F("Host: "), _secure);
    _println(_server, _secure);
    _println(F("Accept: application/json"), _secure);
    _println(F("User-Agent: Arduino-CIOT"), _secure);
    _println(F("access_token: rOxsxbXsj5Zm"), _secure);
    _print(F("id: "), _secure);
    _println(getMacAddress(), _secure);

    if (headers != NULL && headersSize > 0) {
      for (int i=0; i<headersSize;i++) {
        _println(headers[i], _secure);
      }
    }

    if (params != NULL && paramsSize > 0) { // se houver parametros de body, envia os dados na requisição
      String dataString = arrayToJson(params, paramsSize, false);
      int thisLength = dataString.length();

      _println(F("Content-Type: application/json"), _secure);
      _print(F("Content-Length: "), _secure);
      _println(String(thisLength), _secure);
      _println(F("Connection: close"), _secure);
      _println(_secure);
      _println(dataString, _secure);

    } else {
      _println(F("Connection: close"), _secure);
      _println(_secure);
    }

    Serial.println(F("request sent."));

    delay(5);

    long timeoutStart = millis();

    while (!_available(_secure)) {
      // se estourar o timeout limite, cancela e retorna
      if ((millis() - timeoutStart) > 5000) {
        if (_connected(_secure)) {
          _stop(_secure);
        }
        response.responseText = F("failed: response timeout");
        response.statusCode = 666;
        Serial.println(response.responseText);
        return response;
      }
    }

    /*
    if (_VERBOSE_DEBUG) {
      Serial.println(F("response:"));
    }
    */

    response.responseText = STR_EMPTY;
    response.data = STR_EMPTY;

    bool jsonFirstChar = false;
	  bool header = true;
    int readStatus = 3;
    String statusCode = STR_EMPTY;
    while (_available(_secure)) {
      char c = _read(_secure);

      if (response.responseText.indexOf(F("connection: close")) >= 0) {
        header = false;
      }

	  if (defaultRead == readStreaming || header) {
        response.responseText += c;
      }

      // assim que chegar no ponto de leitura do statusCode, seta o contador para pegar os proximos 3 caracteres, correspondentes ao statusCode
      // status: HTTP/1.1 200
      if (readStatus<3) {
        statusCode += c;
        readStatus++;
        if (readStatus == 3) {
          response.statusCode = statusCode.toInt();
        }
      }
      if (response.responseText == F("HTTP/1.0 ") || response.responseText == F("HTTP/1.1 ")) {
        readStatus = 0;
      }

      if (respJson && c == CHAVE) {
        jsonFirstChar = true;
        header = false;
      }

      if (defaultRead == readStreaming && (!respJson || jsonFirstChar)) {
        response.data += c;
      }

      if (!header && !(*readStreaming)(c, &response)) {
        break;
      }
    }

    if (_connected(_secure)) {
      _stop(_secure);
    }
  }

  //Serial.println(response.responseText);
  return response;
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson) {
  return httpRequest(_server, _port, _secure, _method, endpoint, headers, headersSize, params, paramsSize, respJson, defaultRead);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, bool respJson) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int elements, bool respJson) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, params, elements, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, bool respJson) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, params, paramsSize, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int elements, bool respJson) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, params, elements, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, bool respJson) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int elements) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, params, elements, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int elements) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, params, elements, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, bool respJson) {
  return httpRequest(_server, _port, _secure, _method, endpoint, headers, headersSize, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, bool respJson) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, params, elements, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, bool respJson) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, bool respJson) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, params, elements, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, bool respJson) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, NULL, 0, respJson);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize) {
  return httpRequest(_server, _port, _secure, _method, endpoint, headers, headersSize, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, params, elements, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, params, elements, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, NULL, 0, false);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int elements, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, params, elements, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, params, paramsSize, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int elements, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, params, elements, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, read_streaming readStreaming) {
  return httpRequest(_server, _port, _secure, _method, endpoint, NULL, 0, NULL, 0, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int elements, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, params, elements, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, NULL, 0, NULL, 0, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int elements, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, params, elements, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, NULL, 0, NULL, 0, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, _secure, _method, endpoint, headers, headersSize, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, params, elements, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, params, elements, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, NULL, 0, respJson, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming) {
  return httpRequest(_server, _port, _secure, _method, endpoint, headers, headersSize, NULL, 0, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, params, elements, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming) {
  return httpRequest(_server, _port, false, _method, endpoint, headers, headersSize, NULL, 0, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int elements, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, params, elements, false, readStreaming);
}

HttpResponse CiotAdapter::httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming) {
  return httpRequest(_server, 80, false, _method, endpoint, headers, headersSize, NULL, 0, false, readStreaming);
}

// wrapper methods

bool CiotAdapter::_connect(char _server[], int _port, bool _secure) {
  if (_secure) {
    return client_s.connect(_server, _port);
  } else {
    return client.connect(_server, _port);
  }
}

void CiotAdapter::_println(bool _secure) {
  Serial.println();
  if (_secure) {
    client_s.println();
  } else {
    client.println();
  }
}

void CiotAdapter::_print(String str, bool _secure) {
  if (_VERBOSE_DEBUG) {
    Serial.print(str);
  }

  if (_secure) {
    client_s.print(str);
  } else {
    client.print(str);
  }
}

void CiotAdapter::_println(String str, bool _secure) {
  if (_VERBOSE_DEBUG) {
    Serial.println(str);
  }

  if (_secure) {
    client_s.println(str);
  } else {
    client.println(str);
  }
}

bool CiotAdapter::_available(bool _secure) {
  if (_secure) {
    return client_s.available();
  } else {
    return client.available();
  }
}

void CiotAdapter::_stop(bool _secure) {
  if (_secure) {
    client_s.stop();
  } else {
    client.stop();
  }
}

char CiotAdapter::_read(bool _secure) {
  if (_secure) {
    return client_s.read();
  } else {
    return client.read();
  }
}

bool CiotAdapter::_connected(bool _secure) {
  if (_secure) {
    return client_s.connected();
  } else {
    return client.connected();
  }
}

void CiotAdapter::verboseDebug(bool status = true) {
  _VERBOSE_DEBUG = status;
}

void CiotAdapter::enableVerboseDebug() {
  verboseDebug();
}

void CiotAdapter::disableVerboseDebug() {
  verboseDebug(false);
}

void CiotAdapter::fadeLed(int pwmLED) { //unsigned long thisMillis
  unsigned long thisMillis = millis();
  // is it time to update yet?
  // if not, nothing happens
  if (thisMillis - previousFadeMillis >= fadeInterval) {
    // yup, it's time!
    if (fadeDirection == UP) {
      fadeValue = fadeValue + fadeIncrement;
      if (fadeValue >= maxPWM) {
        // At max, limit and change direction
        fadeValue = maxPWM;
        fadeDirection = DOWN;
      }
    } else {
      //if we aren't going up, we're going down
      fadeValue = fadeValue - fadeIncrement;
      if (fadeValue <= minPWM) {
        // At min, limit and change direction
        fadeValue = minPWM;
        fadeDirection = UP;
      }
    }

    analogWrite(pwmLED, fadeValue);
    previousFadeMillis = thisMillis;
  }
}

  void CiotAdapter::ledInitRGB(int statusLedPin_Red, int statusLedPin_Green, int statusLedPin_Blue) {
    _statusLed_HasRGB   = 1;
    _statusLedPin_Red   = statusLedPin_Red;
    _statusLedPin_Green = statusLedPin_Green;
    _statusLedPin_Blue  = statusLedPin_Blue;
    ledsOff();
  }

	void CiotAdapter::ledsOff(){
    if (_statusLed_HasRGB) {
      digitalWrite(_statusLedPin_Red, HIGH);
      digitalWrite(_statusLedPin_Green, HIGH);
      digitalWrite(_statusLedPin_Blue, HIGH);
    }
	}

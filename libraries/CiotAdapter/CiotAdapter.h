#ifndef SENDCIOT_H_
#define SENDCIOT_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>
//#include <avr/pgmspace.h>

typedef struct {
	int statusCode;
	String responseText;
	String data;
} HttpResponse;

typedef void (*log_callback)(void);
typedef bool (*read_streaming)(char c, HttpResponse* response);

class CiotAdapter {
	private:
		void connectToWiFi();
		void startApProvisioning();
		void startDebugMode();
		void turnOffStatusLed();
		void turnOnStatusLed();
		void blinkStatusLed(int times, int lengthms);
		void _println(bool secure);
		void _println(String str, bool secure);
		void _print(String str, bool _secure);
		bool _connect(char _server[], int _port, bool secure);
		bool _available(bool secure);
		void _stop(bool secure);
		char _read(bool secure);
		bool _connected(bool secure);
        static int _fw_version;
		static int _statusLedState;
		static int _configButtonPin;
		static int _currentButtonState;
		static int _buttonSecondsCount;
		static int _errorsCount;
		static byte _previousButtonState;
		static unsigned long _buttonFirstTime;
		static int cont;
		static char* server;
		static int port;
		static bool secure;
		static String text;
		static WiFiClient client;
		static WiFiClientSecure client_s;
		static WiFiManager wifiManager;
		static log_callback logFunction;
		static const char* dataEndpoint;
		static const char* versionEndpoint;
		static bool _VERBOSE_DEBUG;
		static int _statusLedPin;
		static int _statusLedPin_Red;
		static int _statusLedPin_Green;
		static int _statusLedPin_Blue;
		static int _statusLed_HasRGB;
		static int options;
		void _init(short apiVersion);		

	public:
		static const char POST_METHOD[];
		static const char GET_METHOD[];
		static const char PUT_METHOD[];
		static const char DELETE_METHOD[];
		static const char CHAR_EMPTY;
		static const char STR_EMPTY[];
		static const char CHAVE;
		static const char ASPAS_DP[];
		static const int CB_AP_PROVISIONING;
		static const int CB_OTA_UPDATE;
		static const int CB_DEBUG_MODE;
		static const int CB_SMART_CONFIG;
		static const int CB_CONNECT_TO_WIFI;
		static const short V1;
		static const short V2;
		static const short V2_TST;
		static const short V2_STAGE;
		static const short V2_EUROPE;
		static const short V2_PROD;
		static const short V2_TEST;		
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, short apiVersion);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, short apiVersion, int _options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, short apiVersion);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, short apiVersion, int _options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, bool _secure);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, bool _secure);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, int options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, int options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, const char* _dataEndpoint, const char* _versionEndpoint, int options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, bool _secure, int options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, int options);
		CiotAdapter(int fw_version, log_callback f, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, bool _secure, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, int options);
		CiotAdapter(int fw_version, int PinConfigButton, int PinStatusLed, char* _server, int _port, bool _secure, const char* _dataEndpoint, const char* _versionEndpoint, int options);
		bool initialize();
		bool hasConnection();
		HttpResponse sendFullStream(String array[][2], int elements);
		String sendStream(String array[][2], int elements);
		String getStream(String deviceId);
		HttpResponse getStream(String deviceId, bool fullReturn);
		String getStream();
		HttpResponse getFullStream(String deviceId);
		HttpResponse getFullStream();
		HttpResponse getCurrentVersion();
		int processConfigButton();
		String getMacAddress();
		String getIpAddress();
		String getVdd();
		String findText(String text, String startText);
		bool startOTAUpdate();
		bool startOTAUpdate(int OTA_version, String bucket);
		void startSmartConfig();
		String arrayToJson(String array[][2], int size, bool asStr);
		void fadeLed(int pwmLED);
    void ledInitRGB(int statusLedPin_Red, int statusLedPin_Green, int statusLedPin_Blue);
		void ledsOff();
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, bool respJson);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int paramsSize);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int paramsSize);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, bool respJson);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, bool respJson);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String params[][2], int paramsSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String params[][2], int paramsSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, bool respJson, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, bool _secure, const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], int _port, const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, String params[][2], int paramsSize, read_streaming readStreaming);
		HttpResponse httpRequest(char _server[], const char _method[], String endpoint, String headers[], int headersSize, read_streaming readStreaming);
		void verboseDebug(bool status);
		void enableVerboseDebug();
	    void disableVerboseDebug();		
};

#endif

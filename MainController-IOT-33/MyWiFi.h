/*
 Homemade WiFi Library 
 */
#ifndef MyWiFi_h
#define MyWiFi_h


#include "Arduino.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include "MyWiFiSecrets.h" 
//
// Enter the sensitive data in the Secret tab/MyWifi_secrets.h
// 
//char ssid[] = SECRET_SSID;     // your network SSID (name) 
//char pass[] = SECRET_PASS;     // your network password (use for WPA, or use as key for WEP)
//

class MyWiFi
{
public:
 // Constructor 
   MyWiFi(bool displayMsg=false);     // Used to initialize the library and if nothing is specified by main code, pass false to Constructor)
 // Methods
   void begin(int baudRate=115200);   // If nothing is specified by main code, pass default baud rate to method 
   void ConnectToWiFi();              // Determine if connected to WiFi and reconnect if not
   void WiFiFirmwareNotUpToDate();
   void printWifiData(); 
   void printCurrentNet(); 
   void printMacAddress(byte mac[]);
 //
 private: 
   bool _msg;
   int _status = WL_IDLE_STATUS;   // the WiFi radio's status

}; // end class

#endif // end library MyWiFi.h

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
void IsWiFiGood(); // Determine if connected to WiFi and reconnect if not
void WiFiFirmwareNotUpToDate();
void printWifiData(); 
void printCurrentNet(); 
void printMacAddress(byte mac[]);

private: 
int status = WL_IDLE_STATUS;   // the WiFi radio's status

}; // end class

#endif // end library MyWiFi.h

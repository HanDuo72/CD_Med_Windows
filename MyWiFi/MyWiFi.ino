/*
 Homemade WiFi Library 
 */
//#ifndef MyWiFi.h
//#define MyWiFi.h


#include "Arduino.h"
#include <SPI.h>
#include <WiFiNINA.h>

#include "MyWiFiSecrets.h" 
//
// Enter your sensitive data in the Secret tab/arduino_secrets.h
// 
char ssid[] = SECRET_SSID;     // your network SSID (name) 
char pass[] = SECRET_PASS;     // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;   // the WiFi radio's status
unsigned long currentMillis;
unsigned long previousMillis = millis();
int  Delay = 5000;            // Delay period to reset wifi

void setup() 
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
     {
       ; // wait for serial port to connect. Needed for native USB port only
     } // end while

  WiFiFirmwareNotUpToDate();
      
  // attempt to connect to WiFi network:
//     ConnectToWiFi;
        status = WiFi.begin(ssid, pass);       // Connect to WPA/WPA2 network:
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
} // end setup

void loop() 
{
  // check the network connection once every 10 seconds:
  currentMillis = millis();
  if (currentMillis - previousMillis >= Delay)           // If Time delay for checking NTP Server has elapsed
     {
      ConnectToWiFi();                                                                           // Get epoch Set time to NTP Server
      previousMillis = currentMillis;                  // Reset WiFi time delay counter
  
   }  // end if 
} // end loop

void ConnectToWiFi() // Determine if connected to WiFi and reconnect if not
{ 
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  Serial.println("");
  switch (WiFi.status())
    {
     case WL_CONNECTED:       // assigned when connected to a WiFi network;
        // No need to take any action connected to WIFi network
        printCurrentNet();
        printWifiData();
        break;
     case WL_AP_CONNECTED :   // assigned when a device is connected in Access Point mode;
        // Should not be connected in AP Mode.  Disconnect.
        WiFi.disconnect();
        break;
     case WL_AP_LISTENING :   // assigned when the listening for connections in Access Point mode;
        // SHould not be connected in AP Mode.  Disconnect.
        WiFi.disconnect();
        break;
     case WL_NO_SHIELD:       // 255 assigned when no WiFi shield or WL_NO_MODULE is present;
        Serial.println("Communication with WiFi module failed!");
        break;
     case WL_IDLE_STATUS:     // it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        break;
     case WL_NO_SSID_AVAIL:   // assigned when no SSID are available;
        Serial.println("Trying to establish WiFi connection! No SSIDs available");
        break;
     case WL_SCAN_COMPLETED:  // assigned when the scan networks is completed;
        // No need to take any action 
        break;
     case WL_CONNECT_FAILED:  // assigned when the connection fails for all the attempts;
        status = WiFi.begin(ssid, pass);       // Connect to WPA/WPA2 network:
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        break;
     case WL_CONNECTION_LOST: // assigned when the connection is lost;
        status = WiFi.begin(ssid, pass);       // Connect to WPA/WPA2 network:
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        break;
     case WL_DISCONNECTED:    // assigned when disconnected from a network;
        status = WiFi.begin(ssid, pass);       // Connect to WPA/WPA2 network:
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        break;
     default:
        Serial.println("Default Case");
        printCurrentNet();
        printWifiData();
        break;
    } //end switch
 } //end ConnectToWiFi


void WiFiFirmwareNotUpToDate()
{
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
     {
      Serial.print("Installed WiFi Firmware: ");
      Serial.println(fv);
      Serial.print("Most Recent WiFi Firmware: ");
      Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
      Serial.println("Please upgrade the WiFi firmware!");
//      return true;
     } 
     else 
     {
       Serial.print("Installed WiFi Firmware: ");
       Serial.println(fv);
//       return false;
     } // end if...else
} // end WiFiFirmwareUpToDate

void printWifiData() 
{
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
} // end printWifiData

void printCurrentNet() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
} // end printCurrentNet

void printMacAddress(byte mac[]) 
{
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
} // end printMacAddress

//#endif // end library MyWiFi.h

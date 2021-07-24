// Arduino IOT33 Window Light Project
// Main Controller 
// A. Dustin Mets
// Revised 2021.07.16
// https://www.arduino.cc/reference/en/
// https://www.arduino.cc/en/Reference/WiFiNINA
// https://www.arduino.cc/en/Reference/RTC
// https://www.programmingelectronics.com/sprintf-arduino/
// https://forum.arduino.cc/t/leading-zeros-for-seconds-coding/357739/11


// Include required libraries
#include <SPI.h>
#include <WiFiNINA.h>      // For WiFi
#include <WiFiUdp.h>        // For NTP
#include <RTCZero.h>         // For RTC functions
#include <Wire.h>               // Include Arduino Wire library for I2C
 

// WiFi Credentials (edit as required)
char ssid[] = "xxxxxxx";            // Wifi SSID
char pass[] = "xxxxxxxxx";       // Wifi password
int keyIndex = 0;                       // Network key Index number (needed only for WEP)

 
// Real Time Clock
RTCZero rtc;
int status = WL_IDLE_STATUS;

unsigned long LastUpdatedToNTPMillis;                               // Time in ms when we the RTC was last updated to a NTP Server
unsigned long UpdateToNTPDelay = 86400000;                  // 1 day to update time.


// Time zone constant - change as required for your location
const int GMT = 0;             //orig: "const int GMT = -5"    Using 0 to keep it UTC 


//RTC Functions 
void printTime()
{
  print2digits(rtc.getHours() + GMT);
  Serial.print(":");
  
  print2digits(rtc.getMinutes());
  Serial.print(":");
  
  print2digits(rtc.getSeconds());
  Serial.println();
} // end printTime
 
void printDate()
{
  Serial.print(rtc.getDay());
  Serial.print("/");
  
  Serial.print(rtc.getMonth());
  Serial.print("/");
  
  Serial.print(rtc.getYear());
  Serial.print(" ");
} // endprintDate


// WiFi Functions 
void printWiFiStatus()
{
  // Print the network SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // Print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
} // end printWiFiStatus
 




// Global Area (variables are local if declared within { } of loops/functions )

// Window Settings
const int numWindow = 6; // declare and initalize the number of windows as a constant 
//declare and initialize WinX for array reference purposes 
//This may not be necessary if not, can remove later
     int Win1 = 0;
     int Win2 = 1;
     int Win3 = 2;
     int Win4 = 3;
     int Win5 = 4;
     int Win6 = 5;

const String WinName[numWindow] = {"Window1","Window2","Window3","Window4","Window5","Window6"};      //how do I know I always process in window order?
const int WinButtonPin[numWindow] = {2,3,4,5,6,7};                                                                                                                    // declare what ANALOG IO Pin each window button is connected to as a constant

// declare and initialize button high and low readings
const int buttonGreen[2] = {LOW#,HIGH#}
const int buttonBlue[2] = {LOW#,HIGH#}
const int buttonRed[2] = {LOW#,HIGH#}
// use "None" as the default condition in the Case statement


// Internet Post Settings
  char ServerAddress[] = "www.elithecomputerguy.com";
  int ServerPort = 80;
  String postData;
  String postVariable = "temp=";                                // Create a variable to hold the payload - will need to e-rexamine this and customize
  WiFiClient client;                                                        // Creates a client that can connect to to a specified internet IP address and port as defined in client.connect().


// Timing settings
unsigned long currentMillis;                                     // Variable to store the number of milleseconds since the Arduino has started
String currentUTC;                                                      // Variable to store the current datetime in UTC
const int PollIntervalMillis = 50;                               // Do not read pins if less than interval time for polling loop in ms
unsigned long  LongPressMillis = 3000;                  // 3 second for long press  in ms
unsigned long XLongPressMillis = 6000;                 // 6 second for xlong press  in ms
unsigned long XXLongPressMillis = 10000;             // 10 second for xxlong press  in ms

 
unsigned long ButtonLastPolledMillis[numWindow];          // Time in ms when we the button was last polled
unsigned long ButtonPresssedMillis[numWindow];            // Time in ms when we the button was pressed
String ButtonPresssedUTC[numWindow];                             // datetime in UTC when we the button was pressed
unsigned long ButtonPressedDuration[numWindow];       // Total time the button was pressed in ms. Non-Zero value means button was pressed
String ButtonStateCurr[numWindow];                                   // Reflect the current read state of the button
String ButtonStatePrev[numWindow];                                   // Reflect the previous read state of the button


// I2C Communications
const int SLAVE_ADDR[numWindow]  = {9,3,4,5,6,7};      // Define Slave I2C Address
#define ANSWERSIZE 5     // Define Slave I2C answer size








void setup ()
{

// put your setup code here, to run once

  // Start Serial port
  Serial.begin(115200);
 
  // Initialize I2C communications as Master
     Wire.begin();


// Initialize WiFi at boot

     WiFi.disconnect(true);
     delay(1000);

     WiFi.onEvent(Wifi_connected,SYSTEM_EVENT_STA_CONNECTED);
     WiFi.onEvent(Get_IPAddress, SYSTEM_EVENT_STA_GOT_IP);
     WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED); 
     WiFi.begin(ssid, password);
     Serial.println("Waiting for WIFI network...");


  
  // Start Real Time Clock
  rtc.begin();
  unsigned long epoch;                                     // Global Variable to represent epoch
  int numberOfTries = 0, maxTries = 6;          // Variables for number of tries to NTP service
  UpdateTimetoNTPServer()                            // Get epoch Set time to NTP Server

} // End Setup (run once)



void loop()                                                                         
{                                                                                                                             // Put your main code here, to run repeatedly 
 currentMillis = millis();                                                                                     // store the current time in millis
 currentUTC = TimeStampUTC();                                                                     // store the current datetime in UTC
 TimeAdjustments()                                                                                           // Perform Time Functions - Did the Millis reset this loop?  Update NTP 

getWinButtonPinValue();  
compareWinButtonState();
changeWindowLight(); 

}  // end loop


void TimeAdjustments() // Check to see if the Millis have reset and if so reset/clear pressed states
{
// Every 47 days or so the MILLIS reset to 0 and any pending actions will fail 
// Clear all current presses - will result in an odd result for staff - but this will be a very infrequent occurrence 

 if (currentMillis - ButtonLastPolledMillis[1] < 0)                                        // If the difference is negative number then the millis() have reset
     {
     UpdateTimetoNTPServer()                                                                        // Get epoch Set time to NTP Server
     for(int i = 0; i<numWindow; i++)       
          {
            ButtonStatePrev[i] == "None";                                                            // Reset all buttons previous state to not pressed - if currently pressed, it will be detected as a new press and recorded as such for the next loop through
            ButtonLastPolledMillis[i] = currentMillis;
            ButtonPresssedMillis[i] == 0;                                                               // Check this - I don't think this causes a logic problem - but it might. Because only is written when a change is detected so 0 shouldn't matter until press 
                                                                                                                              // is detected and then the 0 will be overwritten
            ButtonPresssedUTC[i] == "";
          } // end for
     }  // end if

If (currentMillis - LastUpdatedToNTPMillis] >= UpdateToNTPDelay)           // If Time delay for checking NTP Server has elapsed
     {
      UpdateTimetoNTPServer();                                                                            // Get epoch Set time to NTP Server
      LastUpdatedToNTPMillis == currentMillis;                                                 // Reset epoch time delay counter
     }  // end if
} // end  TimeAdjustments()


void getWinButtonPinValue()                                                                            // Reads every button pin, determines the current state, preserves previous state
{


 if (currentMillis - ButtonLastPolledMillis[1] >= PollIntervalMillis)             // Only take a reading if the Poll interval has elapsed since last read.
     {
     int WinButtonPinValue[numWindow];                                                       // Declare temp array for to hold each Window's Button Pin Values

          for(int i = 0; i<numWindow; i++)                                                           // Loop to get value of the WinButtonPinValue for each Window
          {
            WinButtonPinValue[i] = analogRead(WinButtonPin[i]);                 // Get Value on ButtonX Pin

             ButtonLastPolledMillis[i] = currentMillis;                                        // Set last polled millis to current millis for next loop 
             ButtonStatePrev[i] = ButtonStateCurr[i];                                        // Copy existing Current State of ButtonX to Previous State of button - prepare to receive new value
             //
             // Set the Current State of the Button
             // Color if pressed, "None" if not 
             //
             switch (WinButtonPinValue[i])  
                  {
                    case 0:
                      ButtonStateCurr[i] = "None";
                      break;
                    case buttonGreen[0] … buttonGreen[1]:
                      ButtonStateCurr[i] = "Green";
                      break;
                    case buttonBlue[0] … buttonBlue[1]:
                      ButtonStateCurr[i] = "Blue";
                      break;
                    case buttonRed[0] … buttonRed[1]:
                      ButtonStateCurr[i] = "Red";
                      break;
                    default:
                      ButtonStateCurr[i] = "None";
                      break;
                  }  // end case
          }  // end for
     }  // end if
}  // end getWinButtonPinValue


void compareWinButtonState()                                                                              // compares current state to previous state takes action
{
          for(int i = 0; i<numWindow; i++)
          {
          if (ButtonStateCurr[i] == ButtonStatePrev[i])                                                  // If Previous and current states Match -
            {                                                                                                                             //  Do nothing - Keep on Keeping on
            } else if (ButtonStateCurr[i] == "None")                                                         //  If current doesn’t match previous and current state is "None" that means button has been released. 
                 {
                                                                                                                                           //Do Something Button was released. 
                  ButtonPressedDuration[i] = currentMillis - ButtonPresssedMillis[i]   // Record total time button was pressed
                 } // end else1
            } else if (ButtonStateCurr[i] != "None")                                                          //  If current state is NOT "None" and doesn’t match previous - that means button has been pressed. 
                 {
                                                                                                                                         // Could have an issue if Previous was Blue and Current is Green - what would happen?
                                                                                                                                          //Do Something - Button was pressed
                  ButtonPresssedMillis[i] = currentMillis                                                   // Record button pressed millis
                  ButtonPresssedUTC[i] = currentUTC                                                       // Record button pressed UTC
                 } // end else2

            } // end if 


Yes - 
No - Is Current None or !None  // Need to do something   
If None - 

               // DOES CURRENT STATE not equal "None"
              If (ButtonStateCurr[i] != "None") 
              {
              else { // Current state equals "None" and Previous State was Non

              }
          }  // end for
}  // end compareWinButtonState


changeWindowLight()
{
 for(int i = 0; i<numWindow; i++)
          {
           switch (ButtonPressedDuration[i])  
                  {
                    case 1 … LongPressMillis:
                             // Action Here ;
                             break;

                    case LongPressMillis … XLongPressMillis:
                             // Action Here ;
                             break;

                    case XLongPressMillis … XXLongPressMillis:
                             // Action Here ;
                             break;

                    default:
                             // DEFAULT Action here;
                             break;
                  }  // end case
          }  // end for
} // end changeWindowLight




void UpdateTimetoNTPServer()                                                            // Get epoch time from NTP Server and update RTC
{
  do 
  {
    epoch = WiFi.getTime();                                                                      // Get Epoch value from NTP Server
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));
 
   if (numberOfTries == maxTries) 
   {
    Serial.print("NTP unreachable!!");
    while (1);
    } else 
      {
       Serial.print("Epoch received: ");
       Serial.println(epoch);
       rtc.setEpoch(epoch);                                                                      // Update RTC to Epoch value
       Serial.println();
    }  // end if…else
} // end UpdateTimetoNTPServer



string TimeStampUTC()                                                          // Returns a UTC formatted string of the current datetime.
{
// Return a string of the current UTC Time Stamp
// 1994-11-05T13:15:30Z     From <https://www.w3.org/TR/NOTE-datetime> 
// YYYY-MM-DDThh:mm:ssZ  <---the Z = UTC or Zulu/Zero time
 string buffer;
 sprintf (buffer, "%04u-%02u-%02uT%02u:%02u:%02uZ", rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours() , rtc.getMinutes(), rtc.getSeconds());
 // The “%02u” means “print an unsigned integer with 2 places, fill with leading zero if necessary”
 return buffer;
} // end TimeStampUTC



Void DataSendToSlave()   // I2C Code
{

  // Write a character to the Slave
  Wire.beginTransmission(SLAVE_ADDR[i]);    // must be in a for loop i
  Wire.write(0);
  Wire.endTransmission();
    
  Serial.println("Receive data");
  
  // Read response from Slave
  // Read back 5 characters
  Wire.requestFrom(SLAVE_ADDR[i],ANSWERSIZE);
  
  // Add characters to string
  String response = "";
  while (Wire.available()) {
      char b = Wire.read();
      response += b;
  } 
  
  // Print to Serial Monitor
  Serial.println(response);


} // End DataSendToSlave



//
//  WiFi Functions 
//
// Using WiFi.onEvent() function for all the three WIFI Events. 
// These will be SYSTEM_EVENT_STA_CONNECTED, SYSTEM_EVENT_STA_GOT_IP and SYSTEM_EVENT_STA_DISCONNECTED. 
// We will individually pass them as the second parameters inside the WiFi.onEvent() function. 
// The previously user defined functions will act as the first parameters.
//  https://techtutorialsx.com/2019/08/15/esp32-arduino-getting-wifi-event-information/
// https://microcontrollerslab.com/reconnect-esp32-to-wifi-after-lost-connection/
//
//
// Broadly speaking, the WifiGenericClass library must be designed so that certain events are automatically handled by the library. 
// When one of these events happen, it looks through the cbEventList to see if any callback functions have been registered for that event. 
// If there is any, it executes them to handle the event. 
// onEvent is not executing the function, it's simply adding it to the list so that it will be called the next time that event happens.
// https://forum.arduino.cc/t/callback-function-without-brackets/616044/10
//

void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Successfully connected to Access Point");
} // end Wifi_connected

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WIFI is connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}  // end Get_IPAddress

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Reconnecting...");
  WiFi.begin(ssid, pass);
} // end Wifi_disconnected


//
//  end WiFi functions Section
//  


//
// Internet Post via JSON payload
//
//
void PostDataToServer()
{
   // if (client.connect(ServerAddress, ServerPort))                    // Make connection to internet server
    // {
    //  Serial.println("connected");
    //  Make a HTTP request:
    //      client.println("GET /search?q=arduino HTTP/1.0");
    //      client.println();
    // Make a HTTP Post
    //      client.println("POST /test/post.php HTTP/1.1");                                            // Sub folder or server location to post to 
    //      client.println("Host: www.elithecomputerguy.com");
    //      client.println("Content-Type: application/x-www-form-urlencoded");
    //      client.print("Content-Length: ");
    //      client.println(postData.length());
    //      client.println();
    //      client.print(postData);
    // Close the HTTP Client
    //     if (client.connected()) 
    //       {
    //        client.stop();
    //       } // end if
   // } // end if
} // end PostDataToServer

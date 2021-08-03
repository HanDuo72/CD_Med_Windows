// Arduino IOT33 Window Light Project
// Window Control Box - Buttons, Lights, Scale 
// A. Dustin Mets
// Revised 2021.08.03
// https://www.arduino.cc/reference/en/
// https://www.arduino.cc/en/Reference/WiFiNINA
// https://www.arduino.cc/en/Reference/RTC
// https://www.programmingelectronics.com/sprintf-arduino/
// https://forum.arduino.cc/t/leading-zeros-for-seconds-coding/357739/11
//
// Include required libraries
#pragma once
#include <SPI.h>
#include <WiFiNINA.h>           // For WiFi
#include <WiFiUdp.h>            // For NTP
#include <RTCZero.h>            // For RTC functions
#include <Wire.h>               // Include Arduino Wire library for I2C
#include <LiquidCrystal_I2C.h>  // Include Library for I2C LCD Screen
#include "MyWiFiSecrets.h"      // Defines SSDI and Password for Wifi
#include "MyWiFi.h"             // Defines my Wifi Functions 
//
// Real Time Clock
  RTCZero rtc;
  unsigned long LastUpdatedToNTPMillis;               // Time in ms when we the RTC was last updated to a NTP Server
  unsigned long UpdateToNTPDelay = 86400000;          // 1 day to update time.
  // Time zone constant - change as required for your location
  const int GMT = 0;                                  // orig: "const int GMT = -5"    Using 0 to keep it UTC 
  unsigned long epoch;                                // Global Variable to represent epoch
  int numberOfTries = 0, maxTries = 6;                // Variables for number of tries to NTP service
  char DateTimeStampUTC[22];
  char DateStamp[10]; 
  char TimeStamp[10]; 
//
// WiFi  
//
// Enter your sensitive data in the Secret tab/MyWiFiSecrets.h
// 
// char ssid[] = SECRET_SSID;             // your network SSID (name) 
// char pass[] = SECRET_PASS;             // your network password (use for WPA, or use as key for WEP)
 int status = WL_IDLE_STATUS;             // the WiFi radio's status
 unsigned long previousWiFiMillis = millis();
 const int  WiFiDelay = 10000;            // Delay period to reset wifi
MyWiFi MyWiFi(true);                      // Instantiate the MyWiFi class    
//
//
//
// Global Area (variables are local if declared within { } of loops/functions )
//
// Window Settings
const String WindowName = "Window 1";
const int WindowNum = 1;
const int ButtonPin = 2;  // !!!!!! This needs checked and updated !!!!!!
// declare and initialize button high and low readings
 const int buttonGreenMin = 1;    // !!!!!! These need checked and updated !!!!!!
 const int buttonGreenMax = 100;    // !!!!!! These need checked and updated !!!!!!
 const int buttonBlueMin = 150;   // !!!!!! These need checked and updated !!!!!!
 const int buttonBlueMax = 250;   // !!!!!! These need checked and updated !!!!!!
 const int buttonRedMin = 300;    // !!!!!! These need checked and updated !!!!!!
 const int buttonRedMax = 400;    // !!!!!! These need checked and updated !!!!!!
// use "None" as the default condition in the Case statement
//
//
// Internet Post Settings
  char ServerAddress[] = "www.elithecomputerguy.com";
  int ServerPort = 80;
  String postData;
  String postVariable = "temp=";                                // Create a variable to hold the payload - will need to e-rexamine this and customize
  WiFiClient client;                                            // Creates a client that can connect to to a specified internet IP address and port as defined in client.connect().
//
//
// Timing settings
 unsigned long currentMillis;                                    // Variable to store the number of milleseconds since the Arduino has started
 String currentUTC;                                              // Variable to store the current datetime in UTC
 const int PollIntervalMillis = 50;                              // Do not read pins if less than interval time for polling loop in ms
 const unsigned long  LongPressMillisMin = 500;                  // .5 seconds for long press Min in ms
 const unsigned long  LongPressMillisMax = 3000;                 // 3 seconds for long press Max in ms
 const unsigned long XLongPressMillisMin = 3001;                 // 3.001 seconds for xlong press Min in ms
 const unsigned long XLongPressMillisMax = 6000;                 // 6 seconds for xlong press Max in ms
 const unsigned long XXLongPressMillisMin = 6001;                // 6.001 seconds for xxlong press Min in ms
 const unsigned long XXLongPressMillisMax = 10000;               // 10 seconds for xxlong press Max in ms

 unsigned long ButtonLastPolledMillis;                           // Time in ms when we the button was last polled
 unsigned long ButtonPresssedMillis;                             // Time in ms when we the button was pressed
 String ButtonPresssedUTC;                                       // datetime in UTC when we the button was pressed
 unsigned long ButtonPressedDuration;                            // Total time the button was pressed in ms. Non-Zero value means button was pressed
 String ButtonStateCurr;                                         // Reflect the current read state of the button
 String ButtonStatePrev;                                         // Reflect the previous read state of the button
//
//
// I2C Communications
 const int SLAVE_ADDR = 9;                              // Define Slave I2C Address
 #define ANSWERSIZE 5                                   // Define Slave I2C answer size
     //
     // I2C LCD Screen
     LiquidCrystal_I2C lcd(0x27, 16, 2);                  // Define LCD I2C Address and Screen size
     unsigned long previousLCDMillis = millis();          // When Screen was LCD last changec
     const int  LCDDelay = 8000;                          // Delay period to change LCD Screen
     const int LCDCarouselScreenMax = 3;                  // Define total number of different Carousel screens
     int LCDCarouselScreen = 1;                           // Varialbe to keep current Carousel screen. 
//
//
//


void setup ()
{
// Start Serial port
  Serial.begin(115200);
 /*
   while (!Serial) 
  {
   ; // wait for serial port to connect. Needed for native USB port only
  } // end while
  */

   Serial.println ("Running Setup");
//  
// Initialize I2C communications as Master
  Wire.begin();
  //Start I2C LCD
    lcd.init();                                        // Start the I2C LCD 
    lcd.backlight();                                   // Turn on the Backlight of the I2C LCD
  // Let the status known to the screen
    lcd.setCursor(0,0);
    lcd.print("Running Setup");
//
// Start the WiFi
  MyWiFi.WiFiFirmwareNotUpToDate();                    // Check to see if the WiFi Firmware is up to date 
  // MyWiFi.ConnectToWiFi();                              // Initial attempt to connect to WiFi network:
  //
  //  This is the older code - Try the function above to see if it works.  If it does, remove the commented out code
  //
   status = WiFi.begin(SECRET_SSID, SECRET_PASS);       // Connect to WPA/WPA2 network:
   Serial.print("Attempting to connect to WPA SSID: ");
   Serial.println(SECRET_SSID);
   lcd.setCursor(0,0);
   lcd.print("Connecting to: ");
   lcd.setCursor(0,1);
   lcd.print(SECRET_SSID);
   delay(10000);
//
// Start Real Time Clock
  rtc.begin();
  UpdateTimetoNTPServer();                          // Get epoch Set time to NTP Server
   Serial.println("Starting RTC");
   lcd.setCursor(0,0);
   lcd.print("Starting RTC");
} // End Setup (run once)



void loop()                                         // Put your main code here, to run repeatedly 
{ 
 currentMillis = millis();                          // store the current time in millis
 UpdateDateTimeStampUTC();                          // Update the TimeStampUTC to the current datetime in UTC
 currentUTC = DateTimeStampUTC;                     // store the current datetime in UTC
 TimeAdjustments();                                 // Perform Time Functions - Did the Millis reset this loop?  Update NTP 
 IsWiFiGood();                                      // Determine if connected to WiFi and reconnect if not
 DisplayCarousel();
 
/*
 getWinButtonPinValue();  
 compareWinButtonState();
 changeWindowLight(); 
*/
}  // end loop

//
// Every 47 days or so the MILLIS reset to 0 and any pending actions will fail 
// Clear all current presses - will result in an odd result for staff - but this will be a very infrequent occurrence 
//
void TimeAdjustments()                                     // Check to see if the Millis have reset and if so reset/clear pressed states
{
 if (currentMillis - ButtonLastPolledMillis < 0)           // If the difference is negative number then the millis() have reset
    {
       Serial.println("Millis Reset");    
       lcd.setCursor(0,0);
       lcd.print("Millis Reset");
       UpdateTimetoNTPServer();                            // Get epoch Set time to NTP Server
       ButtonStatePrev = "None";                           // Reset all buttons previous state to not pressed - if currently pressed, it will be detected as a new press and recorded as such for the next loop through
       ButtonLastPolledMillis = currentMillis;             // Reset last button polled time
       ButtonPresssedMillis = 0;                           // Check this - I don't think this causes a logic problem - but it might. Because only is written when a change is detected so 0 shouldn't matter until press 
                                                           // is detected and then the 0 will be overwritten
       ButtonPresssedUTC = "";                             // Reset button pressed UTC to null
    }  // end if

 if (currentMillis - LastUpdatedToNTPMillis >= UpdateToNTPDelay)           // If Time delay for checking NTP Server has elapsed
     {
      UpdateTimetoNTPServer();                                                                            // Get epoch Set time to NTP Server
      LastUpdatedToNTPMillis == currentMillis;                             // Reset epoch time delay counter
     }  // end if
} // end  TimeAdjustments()


void getButtonPinValue()                                                // Reads every button pin, determines the current state, preserves previous state
{
 if (currentMillis - ButtonLastPolledMillis >= PollIntervalMillis)         // Only take a reading if the Poll interval has elapsed since last read.
     {
       int ButtonPinValue;                                                // Declare local variable for to hold Window's Button Pin Values
       ButtonPinValue = analogRead(ButtonPin);            // Get Value on ButtonX Pin
       ButtonLastPolledMillis = currentMillis;                    // Set last polled millis to current millis for next loop 
       ButtonStatePrev = ButtonStateCurr;                      // Copy existing Current State of ButtonX to Previous State of button - prepare to receive new value
       //
       // Set the Current State of the Button
       // Color if pressed, "None" if not 
       //
          switch (ButtonPinValue)  
           {
             case 0:
               ButtonStateCurr = "None";
               break;
             case buttonGreenMin ...  buttonGreenMax:
               ButtonStateCurr = "Green";
               break;
             case buttonBlueMin ... buttonBlueMax:
               ButtonStateCurr = "Blue";
               break;
             case buttonRedMin ... buttonRedMax:
               ButtonStateCurr = "Red";
               break;
             default:
               ButtonStateCurr = "None";
               break;
           }  // end case
     }  // end if
}  // end getButtonPinValue


void compareWinButtonState()                                                                              // compares current state to previous state takes action
{
  if (ButtonStateCurr == ButtonStatePrev)                              // If Previous and current states Match -
   {   
     ;                                                                 // Do nothing - Keep on Keeping on
   } 
   else if (ButtonStateCurr == "None")                                 // If current doesn’t match previous and current state is "None" that means button has been released. 
   {                                                                   //Do Something Button was released. 
     ButtonPressedDuration = currentMillis - ButtonPresssedMillis;     // Record total time button was pressed
   } // end else1
   else if (ButtonStateCurr != "None")                                 //  If current state is NOT "None" and doesn’t match previous - that means button has been pressed. 
   {                                                                   // Do Something - Button was pressed
     ButtonPresssedMillis[i] = currentMillis;                          // Record button pressed millis
     ButtonPresssedUTC[i] = currentUTC;                                // Record button pressed UTC
    // Could have an issue if Previous was Blue and Current is Green - what would happen?
   } // end else2/if
 

/*
Yes - 
No - Is Current None or !None  // Need to do something   
If None - 

               // DOES CURRENT STATE not equal "None"
              If (ButtonStateCurr[i] != "None") 
              {
              else { // Current state equals "None" and Previous State was Non

              }
          }  // end for
*/
}  // end compareWinButtonState


void changeWindowLight()
{
  switch (ButtonPressedDuration)  
   {
     case LongPressMillisMin ... LongPressMillisMax:
       // Action Here ;
       break;
     case XLongPressMillisMin ... XLongPressMillisMax:
       // Action Here ;
       break;
     case XXLongPressMillisMin ... XXLongPressMillisMax:
       // Action Here ;
       break;
     default:
       // DEFAULT Action here;
       break;
   }  // end case
} // end changeWindowLight

//
//
//  RTC Functions 
//
//
void UpdateDateTimeStampUTC()                                     // Update a UTC formatted string of the current datetime.
  {
    // Return a string of the current UTC Time Stamp
    // 1994-11-05T13:15:30Z     From <https://www.w3.org/TR/NOTE-datetime> 
    // YYYY-MM-DDThh:mm:ssZ  <---the Z = UTC or Zulu/Zero time
    sprintf (DateTimeStampUTC, "%04u-%02u-%02uT%02u:%02u:%02uZ", rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours() , rtc.getMinutes(), rtc.getSeconds());
    // The “%02u” means “print an unsigned integer with 2 places, fill with leading zero if necessary”
  } // end UpdateTimeStampUTC
//
void UpdateTimeStamp()   // was printTime
  {
    sprintf (TimeStamp, "Time %02u:%02u:%02uZ", rtc.getHours() + GMT, rtc.getMinutes(), rtc.getSeconds());
    // The “%02u” means “print an unsigned integer with 2 places, fill with leading zero if necessary”
  } // end printTime
// 
void UpdateDateStamp()  //printDate
  {
    sprintf (DateStamp, "Date %02u/%02u/%04u", rtc.getMonth(), rtc.getDay(), rtc.getYear());
    // The “%02u” means “print an unsigned integer with 2 places, fill with leading zero if necessary”
  } // endprintDate
//
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
//
//
//
//  LCD Screen Functions 
//     Carousel 
//
void DisplayCarousel()
{
   if (currentMillis - previousLCDMillis >= LCDDelay)           // If Time delay for updating LCD Carousel screen has elapsed
     {                                                          // Update to next screen
      if (LCDCarouselScreen == LCDCarouselScreenMax)
        {
          LCDCarouselScreen = 1;                                // Reset to First screen if currently at last screen
        } else
        {
          LCDCarouselScreen++;                                  // Go to next screen in sequenece
        } // end if...esle 
      switch (LCDCarouselScreen)                                // Define each LCD Screen
        {
          case 1:
             lcd.setCursor(0,0);
             lcd.print("This is Screen 1");
             Serial.println("This is Screen 1");
             lcd.setCursor(0,1);
             lcd.print("012345678901234567890");
             Serial.println("012345678901234567890");             
             break;
          case 2:
             lcd.setCursor(0,0);
             lcd.print(DateStamp);
             Serial.println(DateStamp);
             lcd.setCursor(0,1);
             lcd.print(TimeStamp);
             Serial.println(TimeStamp;
             break;
          case 3:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("This is Screen 3");
             Serial.println("This is Screen 3");             
             lcd.setCursor(0,1);
             lcd.print(WiFi.localIP());
             Serial.println(WiFi.localIP());
             // ButtonStateCurr[i] = "None"; WiFi.localIP()
             break;
          default:
             lcd.clear();
             Serial.println("This is Default Case - Clear LCD Screen");                
             // Set LCD to blank - Clear
             break;
        }  // end switch
      previousLCDMillis = currentMillis;                       // Reset LCD Carousel  time delay counter
     }  // end if
} // end DisplayCarousel



/*
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
*/


//
//  WiFi Functions 
//

void IsWiFiGood()  // check the network connection once every WiFiDelay seconds:
  { 
    if (currentMillis - previousWiFiMillis >= WiFiDelay)           // If Time delay for checking NTP Server has elapsed
    {
      MyWiFi.ConnectToWiFi();                                                                           // Get epoch Set time to NTP Server
      previousWiFiMillis = currentMillis;                  // Reset WiFi time delay counter
    }  // end if 
  } // end IsWiFiGood

//
// The rest of the functions are in MyWiFi.h if they work.  If not, paste them in here
//
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

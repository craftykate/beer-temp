/*
 * BEER TEMP SENSOR
 * Author: Kate McFaul
 * Date: August 2018
 * 
 * Log temperature of beer's water bath to thingspeak.com and serial monitor if connected every 5 minutes
 * If temperature falls outside of acceptable range, text Kevin
 * Change RGB LED: Red for too hot, Blue for too cold, Green if fine
 * 
 * INSTRUCTIONS:
 * Change lowerRange and upperRange variables 
 * Ground Pin 0 before uploading new code. Undo when operating in the wild
 */


// INCLUDE LIBRARIES
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <ESP8266WiFi.h> 
#include <ThingSpeak.h>

// SET UP VARIABLES
// Acceptable temp range
const int lowerRange = 70; // <========== CHANGE THIS
const int upperRange = 75; // <========== CHANGE THIS
int lowestTemp = lowerRange;
int highestTemp = upperRange;
bool outOfRange = false;
// LED pins
int RedPin = 12;
int GreenPin = 13;
int BluePin = 16;
// Blinking variables
int ledState = LOW;
unsigned long previousBlinkMillis = 0; // When led last changed state
unsigned long blinkInterval = 2000; // Blink LED every 2 seconds
// ThingSpeak variables
unsigned long previousThingMillis = 0; // When ThingSpeak was last connected to
unsigned long thingInterval = 300000; // Connect to ThingSpeak every 5 minutes (300000 ms) (must be at least 15 sec according to ThingSpeak)
// Onboard pin
#define LED_PIN 5
// Set up WiFi object
WiFiClient client;
// Variable to store current temp
int temp; // Change this to float instead of int if you want decimals. This will send a text with a number like 82.50000

// WiFi and Channel parameters 
const char WIFI_SSID[] = "<YOUR WIFI SSID>";
const char WIFI_PSK[] = "<YOUR WIFI PASSWORD>";
unsigned long CHANNEL_ID = <THINGSPEAK CHANNEL ID>;
const char * WRITE_API_KEY = "<THINGSPEAK WRITE API KEY>";

// TEMP SENSOR SET UP
// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // Set up onboard LED for debugging
  pinMode(LED_PIN, OUTPUT);

  // Set up RGB LED
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  
  // Start serial port
  Serial.begin(9600);

  // Start up the temperature library
  sensors.begin();

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);
}

void loop(void)
{ 
  // Store current time (well, how much time has passed since program started)
  unsigned long currentMillis = millis();

  // CHECK TEMP, POST TO THINGSPEAK
  // Check if enough time has passed to check temp
  if(currentMillis - previousThingMillis >= thingInterval || previousThingMillis == 0) {
    // Replace last time with current time
    previousThingMillis = currentMillis;
    
    // Flash LED to show that we're sampling
    digitalWrite(LED_PIN, LOW);
  
    // GET TEMP
    // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
    Serial.print("Requesting temperatures...");
    // Send the command to get temperatures
    sensors.requestTemperatures(); 
    // Use the function ByIndex, and get the temperature from the first sensor only
    temp = sensors.getTempFByIndex(0);
    Serial.println("DONE");
    
    // PRINT TEMP TO SERIAL
    Serial.print("Temperature is: ");
    Serial.println(temp);  
    Serial.print("Acceptable temp range: ");
    Serial.print(lowerRange);
    Serial.print(" to ");
    Serial.println(upperRange);
    Serial.println("");
  
    // ADD TEMP TO THINGSPEAK
    // Set Temp field
    ThingSpeak.setField(1, temp);
    // Send range values
    ThingSpeak.setField(4, lowerRange);
    ThingSpeak.setField(5, upperRange);
    // If temp is out of range, set outOfRange to true
    if(temp < lowerRange || temp > upperRange) {
      outOfRange = true;
    }
    // If temp is in low range and lower than it has been
    if(temp < lowestTemp) { 
      // Set lowestTemp to current temp
      lowestTemp = temp;
      // Make sure higher temp variable is reset
      highestTemp = upperRange;
      // Trigger another text
      ThingSpeak.setField(2, 1); 
      // Make sure other text triggers are reset
      ThingSpeak.setField(3, 0);
      ThingSpeak.setField(6, 0);
    }
    // If temp is in good range text if it's the first time back and make sure all variables are reset
    if(temp >= lowerRange && temp <= upperRange) {
      // If it was out of range and now is not...
      if(outOfRange == true) {
        ThingSpeak.setField(6, 1);
      }
      lowestTemp = lowerRange;
      highestTemp = upperRange;
      outOfRange = false;
      ThingSpeak.setField(2, 0);
      ThingSpeak.setField(3, 0);
    }
    // If temp is in high range and higher than it has been
    if(temp > highestTemp) {
      // Set highestTemp to current temp
      highestTemp = temp;
      // Make sure lower temp variable is reset
      lowestTemp = lowerRange;
      // Trigger another text
      ThingSpeak.setField(3, 1); 
      // Make sure other text triggers are reset
      ThingSpeak.setField(2, 0);
      ThingSpeak.setField(6, 0);
    }
    // Write all fields to ThingSpeak
    ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
    
    // Turn LED off after posting data
    digitalWrite(LED_PIN, HIGH);
  }

  // BLINK LED THE APPROPRIATE COLOR
  // Check if enough time has passed to blink led
  if(currentMillis - previousBlinkMillis >= blinkInterval || previousBlinkMillis == 0) {
    // Replace last time with current time
    previousBlinkMillis = currentMillis;

    // Toggle whether led should be on or off
    if(ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    
    if(temp < lowerRange) { // If temp is too low toggle blue led
      digitalWrite(RedPin, LOW);
      digitalWrite(GreenPin, LOW);
      digitalWrite(BluePin, ledState);
    }
    if(temp >= lowerRange && temp <= upperRange) { // If temp is in range toggle green led
      digitalWrite(RedPin, LOW);
      digitalWrite(GreenPin, ledState);
      digitalWrite(BluePin, LOW);
    }
    if(temp > upperRange) { // If temp is too high toggle red led
      digitalWrite(RedPin, ledState);
      digitalWrite(GreenPin, LOW);
      digitalWrite(BluePin, LOW);
    }
  }

  // Give program time to do its wifi thing
  delay(100); 
}

// Attempt to connect to WiFi
void connectWiFi() {

  byte led_status = 0;

  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);

  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Blink LED while waiting for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
  }

  // Turn LED off when connected
  digitalWrite(LED_PIN, HIGH);
}
